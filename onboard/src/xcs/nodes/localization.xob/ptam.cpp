#include "ptam.hpp"

#include "tum/tum_utils.hpp"

#include <TooN/TooN.h>

#include <ptam/Tracker.h>
#include <ptam/ATANCamera.h>
#include <ptam/Map.h>
#include <ptam/MapMaker.h>
#include <ptam/GLWindow2.h>
#include <ptam/MouseKeyHandler.h>

#include <xcs/logging.hpp>
#include <xcs/xcs_fce.hpp>


using namespace std;
using namespace xcs;
using namespace xcs::nodes;
using namespace xcs::nodes::localization;

void Ptam::on_key_down(int key) {
    if (key == 32) { // space
        if (ptamReady_) {
            ptamTracker_->pressSpacebar();
        }
    }
}

Ptam::Ptam(Ekf &ekf) :
  ptamReady_(false),
  ekf_(ekf) {
    /* 
     * Default values
     */
    parameters_["showWindow"] = 0;
    parameters_["frameWidth"] = 640;
    parameters_["frameHeight"] = 360;
    // other defaults to zero

    cameraParameters_.resize(5);

    frameNo_ = 0;
    updateFixpoint_ = false;
    mapLocked_ = false;
    forceKF_ = false;
    // framesForScaleInterval_ = -1; NOTE: not initialized
}

Ptam::~Ptam() {
}

void Ptam::cameraParameters(const CameraParameters& values) {
    cameraParameters_ = values; // TODO refresh camera?
}

void Ptam::parameters(const Parameters& values) {
    parameters_ = values;
    resetPtam();
}

inline void dbgPrintSE3(const string &tag, const TooN::SE3<> &pose, const double timestamp) {
    const TooN::Vector<3> angles = so3ToAngles(pose.get_rotation());
    const auto translation = pose.get_translation();
    XCS_LOG_INFO("PTAM (" << tag << "): " << translation[0] << " " << translation[1] << " " << translation[2] << " " << angles[0] << " " << angles[1] << " " << angles[2] << " " << timestamp);
}

void Ptam::handleFrame(::urbi::UImage &bwImage, Timestamp timestamp) {
    /*
     * This runs in UNotifyThreaded and it probably uses a thread pool,
     * this ensures consistent access to PTAM objects (that may be being
     * destroyed in a different thread).
     */
    if (!ptamReady_) {
        return;
    }

    if (resetPtamRequested_) {
        resetPtam();
    }

    /*
     * Copy image data to CVD representation
     */
    frameNo_ += 1;
    if (frame_.size().x != bwImage.width || frame_.size().y != bwImage.height) {
        frame_.resize(CVD::ImageRef(bwImage.width, bwImage.height));
    }

    memcpy(frame_.data(), bwImage.data, bwImage.width * bwImage.height);


    /*
     * Update GUI
     */
    if (showWindow_) {
        glWindow_->activate(); // set context for the current thread
        glWindow_->SetupViewport();
        glWindow_->SetupVideoOrtho();
        glWindow_->SetupVideoRasterPosAndZoom();
    }

    /*
     * Obtain predicted state from EKF, transform it to PTAM's system
     * and track the frame.
     */
    const TooN::Vector<10> filterPosePrePTAM = ekf_.computeState(timestamp);
    const SE3Element droneToWPred(vectorToSe3(filterPosePrePTAM.slice<0, 6>())); // only pose
    const SE3Element worldUToCam(scaleEstimation_.droneToFront * droneToWPred.inverse() * scaleEstimation_.offsetMatrix()); // unscaled word to camera
    const SE3Element ptamToCamPPred(worldUToCam.get_rotation(), worldUToCam.get_translation() / scaleEstimation_.scale()); // PTAM (scaled) world to camera

    ptamTracker_->setPredictedCamFromW(ptamToCamPPred);

    ptamTracker_->setLastFrameLost((goodCount_ < -20), (frameNo_ % 3 == 0)); // TODO (Michal): What is this?

    // track
    ptamTracker_->TrackFrame(frame_, showWindow_);
    const SE3Element ptamToCamPResult = ptamTracker_->GetCurrentPose();
    const SE3Element camPToPtamResult(ptamToCamPResult.inverse());
    const SE3Element camToWorldU(camPToPtamResult.get_rotation(), camPToPtamResult.get_translation() * scaleEstimation_.scale());
    const SE3Element droneToWResult(scaleEstimation_.offsetMatrix() * camToWorldU * scaleEstimation_.droneToFront);

    scaleEstimation_.initializeOffset(droneToWPred * droneToWResult.inverse()); // this must be before initializeScale

    dbgPrintSE3("newoff", scaleEstimation_.offsetMatrix(), timestamp);
    dbgPrintSE3("dronetowp", droneToWPred, timestamp);
    dbgPrintSE3("dronetowr", droneToWResult, timestamp);
    dbgPrintSE3("offset", scaleEstimation_.offsetMatrix(), timestamp);


    /*
     * Handle initial phase of PTAM.
     */
    if (ptamTracker_->lastStepResult == ptamTracker_->I_FAILED) {
        XCS_LOG_INFO("Initializing PTAM failed, resetting.");
        resetPtamRequested_ = true;
    }
    if (ptamTracker_->lastStepResult == ptamTracker_->I_SECOND) {
        scaleEstimation_.initializeScale(ptamMapMaker_->initialScaleFactor);
        ptamMapMaker_->currentScaleFactor = ptamMapMaker_->initialScaleFactor; // workaround: for correct metric distance in MapMapek

        XCS_LOG_INFO("PTAM initialized with initial scale " << scaleEstimation_.scale() << ".");
        framesForScaleInterval_ = -1;
        updateFixpoint_ = true; // TODO what is this
    }
    if (ptamTracker_->lastStepResult == ptamTracker_->I_FIRST) {
        XCS_LOG_INFO("PTAM initialization started (took first KF)");
    }



    /*
     * Assess quality of tracking from prediction and result difference.
     */
    bool isGood = true;
    bool isVeryGood = true;
    // calculate absolute differences.
    TooN::Vector<6> diffs; // 
    diffs.slice<0, 3>() = droneToWResult.get_translation() - droneToWPred.get_translation();
    diffs.slice<3, 3>() = so3ToAngles(droneToWResult.get_rotation()) - so3ToAngles(droneToWPred.get_rotation());

    for (int i = 0; i < 6; i++) {
        diffs[i] = abs(diffs[i]);
    }


    if (goodObservations_ < 10 && ptamMap_->IsGood()) { // map must be created (note it's created in different thread)
        isGood = true;
        isVeryGood = false;
    } else if (ptamTracker_->lastStepResult == ptamTracker_->I_FIRST ||
            ptamTracker_->lastStepResult == ptamTracker_->I_SECOND ||
            ptamTracker_->lastStepResult == ptamTracker_->I_FAILED ||
            ptamTracker_->lastStepResult == ptamTracker_->T_LOST ||
            ptamTracker_->lastStepResult == ptamTracker_->NOT_TRACKING ||
            ptamTracker_->lastStepResult == ptamTracker_->INITIALIZING)
        isGood = isVeryGood = false;
    else {
        // Engel: some chewy heuristic when to add and when not to.
        bool dodgy = ptamTracker_->lastStepResult == ptamTracker_->T_DODGY ||
                ptamTracker_->lastStepResult == ptamTracker_->T_RECOVERED_DODGY;

        /*
         * Engel: If yaw difference too big: something certainly is wrong.
         * Engel: maximum difference is 10° + 2° * (number of seconds since PTAM observation).
         */
        double maxYawDiff = degreesToRadians(10) + degreesToRadians(2) * duration2sec(Clock::now() - lastGoodYawClock_);
        if (maxYawDiff > degreesToRadians(20)) {
            maxYawDiff = degreesToRadians(1000);
        }
        if (diffs[5] > maxYawDiff) {
            isGood = false;
        }

        if (diffs[5] < degreesToRadians(10)) {
            lastGoodYawClock_ = Clock::now();
        }
        if (diffs[5] > degreesToRadians(4)) {
            isVeryGood = false;
        }

        /*
         * Engel: if rp difference too big: something certainly is wrong.
         */
        if (diffs[3] > degreesToRadians(20) || diffs[4] > degreesToRadians(20)) {
            isGood = false;
        }

        if (diffs[3] > degreesToRadians(3) || diffs[4] > degreesToRadians(3) || dodgy) {
            isVeryGood = false;
        }
    }

    if (isGood) {
        if (goodCount_ < 0) {
            goodCount_ = 0;
        }
        goodCount_++;
    } else {
        if (goodCount_ > 0) {
            goodCount_ = 0;
        }
        goodCount_--;

        // Michal: I don't understand what is this good for
        if (ptamTracker_->lastStepResult == ptamTracker_->T_RECOVERED_DODGY) {
            goodCount_ = std::max(goodCount_, -2);
        }
        if (ptamTracker_->lastStepResult == ptamTracker_->T_RECOVERED_GOOD) {
            goodCount_ = std::max(goodCount_, -5);
        }
    }

    /*
     * If the result is good and stable push it to EKF.
     */
    XCS_LOG_INFO("PTAM (metadata): " << timestamp << " " << ptamTracker_->lastStepResult << " " << static_cast<int> (goodCount_ >= 3));

    if (goodCount_ >= 3) {
        ekf_.measurementCam(droneToWResult, timestamp);
        goodObservations_ += 1;
    } else {
        ekf_.clearUpToTime(timestamp);
    }

    /*
     * Scale estimation (in intervals)
     * 
     * Engel:
     *   interval length is always between 1s and 2s, to enshure approx. same variances.
     *   if interval contained height inconsistency, z-distances are simply both set to zero, which does not generate a bias.
     *   otherwise distances are scaled such that height is weighted more.
     *   if isGood>=3 && framesIncludedForScale < 0			===> START INTERVAL  
     *   if 18 <= framesIncludedForScale <= 36 AND isGood>=3	===> ADD INTERVAL, START INTERVAL
     *   if framesIncludedForScale > 36						===> set framesIncludedForScale=-1 
     */


    // if interval is started: add one step.
    if (framesForScaleInterval_ >= 0) {
        framesForScaleInterval_++;
    }

    // if interval is overdue: reset & dont add
    const double includedTime = timestamp - ptamPositionForScaleTakenTimestamp_;
    if (includedTime > 3.0) {
        framesForScaleInterval_ = -1;
    }

    // result of PTAM is new scalingFixpoint (these are coordinates in PTAM world)
    const TooN::Vector<3> scalingFixpoint((scaleEstimation_.offsetMatrix().inverse() * droneToWResult).get_translation() / scaleEstimation_.scale());

    if (goodCount_ >= 3) {
        // position in PTAM (scaled) coords
        const TooN::Vector<3> ptamPositionForScale(droneToWResult.get_translation() / scaleEstimation_.scale());

        if (includedTime >= 2.0 && framesForScaleInterval_ > 1) { // we're ready to process an interval
            framesForScaleInterval_ = -1; // causing reset afterwards

            TooN::Vector<3> diffPTAM = ptamPositionForScale - lastPtamPositionForScale_;

            bool zCorrupted, allCorrupted;
            TooN::Vector<3> diffIMU = evalNavQue(ptamPositionForScaleTakenTimestamp_, timestamp, &zCorrupted, &allCorrupted);

            if (!allCorrupted) {
                // filtering: z more weight, but only if not corrupted.
                const double xyFactor = 0.05;
                const double zFactor = zCorrupted ? 0 : 3;

                diffPTAM.slice<0, 2>() *= xyFactor;
                diffPTAM[2] *= zFactor;
                diffIMU.slice<0, 2>() *= xyFactor;
                diffIMU[2] *= zFactor;

                scaleEstimation_.updateScale(diffPTAM, diffIMU, scalingFixpoint);
                ptamMapMaker_->currentScaleFactor = scaleEstimation_.scale();
                XCS_LOG_INFO("New scale: " << ptamMapMaker_->currentScaleFactor << " from diffs: " << diffPTAM << "; " << diffIMU << ", times " << ptamPositionForScaleTakenTimestamp_ << ", " << timestamp);
            }
        }

        // begin a new interval
        if (framesForScaleInterval_ == -1) {
            framesForScaleInterval_ = 0;
            lastPtamPositionForScale_ = ptamPositionForScale;
            ptamPositionForScaleTakenTimestamp_ = timestamp;
        }
    }


    if (updateFixpoint_ && isGood) {
        scaleEstimation_.scalingFixpoint(scalingFixpoint);
        updateFixpoint_ = false;
        // TODO: (Michal) I don't know whether to use or not the fixpoint

        XCS_LOG_INFO("Locking scale fixpoint to " << scalingFixpoint[0] << " " << scalingFixpoint[1] << " " << scalingFixpoint[2]);
    }


    // ----------------------------- Take KF? -----------------------------------
    if (!mapLocked_ && isVeryGood && (forceKF_ || ptamMap_->vpKeyFrames.size() < maxKF_ || maxKF_ <= 1)) {
        ptamTracker_->TakeKF(forceKF_);
        forceKF_ = false;
    }

    /*
     * Set correct status for outer user.
     */
    if (ptamTracker_->lastStepResult == ptamTracker_->NOT_TRACKING) {
        ptamStatus_ = PTAM_IDLE;
    } else if (ptamTracker_->lastStepResult == ptamTracker_->I_FIRST ||
            ptamTracker_->lastStepResult == ptamTracker_->I_SECOND ||
            ptamTracker_->lastStepResult == ptamTracker_->T_TOOK_KF) {
        ptamStatus_ = PTAM_TOOKKF;
    } else if (ptamTracker_->lastStepResult == ptamTracker_->INITIALIZING) {
        ptamStatus_ = PTAM_INITIALIZING;
    } else if (isVeryGood) {
        ptamStatus_ = PTAM_BEST;
    } else if (isGood) {
        ptamStatus_ = PTAM_GOOD;
    } else if (ptamTracker_->lastStepResult == ptamTracker_->T_DODGY ||
            ptamTracker_->lastStepResult == ptamTracker_->T_GOOD) {
        ptamStatus_ = PTAM_FALSEPOSITIVE;
    } else {
        ptamStatus_ = PTAM_LOST;
    }

    /*
     * Handle window (at the end bacause of events)
     */
    if (showWindow_) {
        glWindow_->DrawCaption(ptamTracker_->GetMessageForUser());
        glWindow_->DrawMenus();
        glWindow_->swap_buffers();
        glWindow_->HandlePendingEvents();
    }
}

void Ptam::measurementImu(const DroneStateMeasurement measurement, const double timestamp) {
    lock_guard<mutex> lock(imuMeasurementsMtx_);
    ImuMeasurementChronologic copyMeasurement(measurement, timestamp);
    imuMeasurements_.push_back(copyMeasurement);
}

void Ptam::pressSpacebar() {
    ptamTracker_->pressSpacebar();
}

TooN::Vector<3> Ptam::evalNavQue(Timestamp from, Timestamp to, bool* zCorrupted, bool* allCorrupted) {
    // TODO emptying queue
    Timestamp firstAdded = 0, lastAdded = 0;
    double firstZ = 0;
    predIMUOnlyForScale_.resetPos();
    int samples = 0;

    { // lock the queue
        lock_guard<mutex> lock(imuMeasurementsMtx_);
        for (ImuMeasurements::iterator it = imuMeasurements_.begin(); it != imuMeasurements_.end(); ++it) {
            auto frontStamp = it->second;
            if (frontStamp < from) // packages before: delete
            {
                //navInfoQueue.pop_front();
            } else if (frontStamp >= from && frontStamp <= to) {
                if (firstAdded == 0) {
                    firstAdded = frontStamp;
                    firstZ = it->first.altitude;
                    predIMUOnlyForScale_.z = firstZ; // avoid height check initially!
                }
                lastAdded = frontStamp;
                // add
                predIMUOnlyForScale_.predictOneStep(*it);
                samples += 1;
                // pop
                //navInfoQueue.pop_front();
            } else {
                break;
            }
        }

    }
    cout << from << ";" << to << " " << firstAdded << ";" << lastAdded << " " << samples << endl;
    //printf("QueEval: before: %i; skipped: %i, used: %i, left: %i\n", totSize, skipped, used, navInfoQueue.size());
    predIMUOnlyForScale_.z -= firstZ; // make height to height-diff

    *zCorrupted = predIMUOnlyForScale_.zCorrupted;
    *allCorrupted = std::abs(firstAdded - from) + std::abs(lastAdded - to) > 0.08; // 80 ms //TODO check this abs'es are correct

    return TooN::makeVector(predIMUOnlyForScale_.x, predIMUOnlyForScale_.y, predIMUOnlyForScale_.z);
}

void Ptam::resetPtam() {
    ptamReady_ = false;
    int frameWidth = static_cast<int> (parameters_["frameWidth"]);
    int frameHeight = static_cast<int> (parameters_["frameHeight"]);
    showWindow_ = parameters_["showWindow"] > 0;

    if (showWindow_) {
        if (!glWindow_) {
            glWindow_ = GLWindow2Ptr(new GLWindow2(CVD::ImageRef(frameWidth, frameHeight), "PTAM Drone Camera Feed", this));
        }
    } else {
        glWindow_.reset();
    }


    // (re) create all PTAM instances
    ptamCamera_ = ATANCameraPtr(new ATANCamera(cameraParameters_));
    ptamMap_ = MapPtr(new Map());
    ptamMapMaker_ = MapMakerPtr(new MapMaker(*ptamMap_, *ptamCamera_));
    ptamTracker_ = TrackerPtr(new Tracker(CVD::ImageRef(frameWidth, frameHeight), *ptamCamera_, *ptamMap_, *ptamMapMaker_));

    ptamMapMaker_->minKFDist = parameters_["minKFDist"];
    ptamMapMaker_->minKFWiggleDist = parameters_["minKFWiggleDist"];
    ptamTracker_->minKFTimeDist = parameters_["minKFTimeDist"];
    maxKF_ = static_cast<int> (parameters_["maxKF"]);

    predIMUOnlyForScale_.resetPos();

    goodCount_ = 0;
    forceKF_ = false;
    goodObservations_ = 0;
    updateFixpoint_ = false;
    ptamStatus_ = PTAM_IDLE;

    resetPtamRequested_ = false;
    ptamReady_ = true;
}