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

const int Ptam::FRAME_WIDTH = 640;
const int Ptam::FRAME_HEIGHT = 360;

void Ptam::on_key_down(int key) {
    if (key == 32) { // space
        ptamTracker_->pressSpacebar();
    }
}

Ptam::Ptam(Ekf &ekf) :
  ekf_(ekf),
  showWindow_(true) {
    // TODO load camera parameters from configuration (these are for AR Drone Parrot 2.0)
    cameraParameters_[0] = 0.771557;
    cameraParameters_[1] = 1.368560;
    cameraParameters_[2] = 0.552779;
    cameraParameters_[3] = 0.444056;
    cameraParameters_[4] = 1.156010;

    // initialization    
    resetPtam();

    if (showWindow_) {
        glWindow_ = GLWindow2Ptr(new GLWindow2(CVD::ImageRef(FRAME_WIDTH, FRAME_HEIGHT), "PTAM Drone Camera Feed", this));
    }

    frameNo_ = 0;
    lockNextFrame_ = false;
    mapLocked_ = false;
    forceKF_ = false;
    // framesIncludedForScaleXYZ_ = -1; NOTE: not initialized
    maxKF_ = 60;

}

Ptam::~Ptam() {
}

inline void dbgPrintSE3(const string &tag, const TooN::SE3<> &pose, const double timestamp) {
    const TooN::Vector<3> angles = so3ToAngles(pose.get_rotation());
    const auto translation = pose.get_translation();
    XCS_LOG_INFO("PTAM (" << tag << "): " << translation[0] << " " << translation[1] << " " << translation[2] << " " << angles[0] << " " << angles[1] << " " << angles[2] << " " << timestamp);
}

void Ptam::handleFrame(::urbi::UImage &bwImage, Timestamp timestamp) {
    frameNo_ += 1;

//    if (frameNo_ % 4 != 0) { // TODO: slow machine, only half a frame rate
//        return;
//    }

    if (resetPtamRequested_) {
        resetPtam();
    }

    /*
     * Copy image data to CVD representation
     */
    if (frame_.size().x != bwImage.width || frame_.size().y != bwImage.height) {
        frame_.resize(CVD::ImageRef(bwImage.width, bwImage.height));
    }

    memcpy(frame_.data(), bwImage.data, bwImage.width * bwImage.height);

    TooN::Vector<10> filterPosePrePTAM = ekf_.computeState(timestamp); // TODO (michal): use appropriate structure for state

    // ------------------------ do PTAM -------------------------
    if (showWindow_) {
        glWindow_->activate(); // set context for the current thread
        glWindow_->SetupViewport();
        glWindow_->SetupVideoOrtho();
        glWindow_->SetupVideoRasterPosAndZoom();
    }

    const SE3Element droneToWPred(vectorToSe3(filterPosePrePTAM.slice<0, 6>()));
    const SE3Element worldUToCam(scaleEstimation_.droneToFront * droneToWPred.inverse() * scaleEstimation_.offsetMatrix());
    const SE3Element ptamToCamPPred(worldUToCam.get_rotation(), worldUToCam.get_translation() / scaleEstimation_.scale());

    // set
    ptamTracker_->setPredictedCamFromW(ptamToCamPPred);

    //ptamTracker_->setLastFrameLost((goodCount_ < -10), (videoFrameID%2 != 0));
    ptamTracker_->setLastFrameLost((goodCount_ < -20), (frameNo_ % 3 == 0)); // TODO (Michal): What is this?

    // track
    ptamTracker_->TrackFrame(frame_, showWindow_);
    const SE3Element ptamToCamPResult = ptamTracker_->GetCurrentPose();
    const SE3Element camPToPtamResult(ptamToCamPResult.inverse());
    const SE3Element camToWorldU(camPToPtamResult.get_rotation(), camPToPtamResult.get_translation() * scaleEstimation_.scale());

    const SE3Element droneToWResult(scaleEstimation_.offsetMatrix() * camToWorldU * scaleEstimation_.droneToFront);
    dbgPrintSE3("dronetowp", droneToWPred, timestamp);
    dbgPrintSE3("dronetowr", droneToWResult, timestamp);



    // init failed?
    if (ptamTracker_->lastStepResult == ptamTracker_->I_FAILED) {
        XCS_LOG_INFO("initializing PTAM failed, resetting!");
        resetPtamRequested_ = true;
    }
    if (ptamTracker_->lastStepResult == ptamTracker_->I_SECOND) {
        //PTAMInitializedClock = getMS(); TODO this is used in MapView to redraw something(?) only after 200 ms
        scaleEstimation_.initializeScale(ptamMapMaker_->initialScaleFactor); // TODO (Michal): use initial scale factor = 1
        ptamMapMaker_->currentScaleFactor = ptamMapMaker_->initialScaleFactor;

        XCS_LOG_INFO("PTAM initialized!");
        XCS_LOG_INFO("initial scale: " << scaleEstimation_.scale());
        //node->publishCommand("u l PTAM initialized (took second KF)"); // TODO what is this for
        framesIncludedForScaleXYZ_ = -1;
        lockNextFrame_ = true;
    }
    if (ptamTracker_->lastStepResult == ptamTracker_->I_FIRST) {
        XCS_LOG_INFO("PTAM initialization started (took first KF)");
    }






    // --------------------------- assess result ------------------------------
    bool isGood = true;
    bool isVeryGood = true;
    // calculate absolute differences.
    TooN::Vector<6> diffs;
    diffs.slice<0, 3>() = droneToWResult.get_translation() - droneToWPred.get_translation();
    diffs.slice<3, 3>() = so3ToAngles(droneToWResult.get_rotation()) - so3ToAngles(droneToWPred.get_rotation());

    for (int i = 0; 1 < 1; i++) {
        diffs[i] = abs(diffs[i]);
    }


    if (goodObservations_ < 10 && ptamMap_->IsGood()) {
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
        // some chewy heuristic when to add and when not to.
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
        if (false && diffs[5] > maxYawDiff) {
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

        if (ptamTracker_->lastStepResult == ptamTracker_->T_RECOVERED_DODGY)
            goodCount_ = std::max(goodCount_, -2);
        if (ptamTracker_->lastStepResult == ptamTracker_->T_RECOVERED_GOOD)
            goodCount_ = std::max(goodCount_, -5);

    }

    // --------------------------- scale estimation & update filter (REDONE) -----------------------------
    // interval length is always between 1s and 2s, to enshure approx. same variances.
    // if interval contained height inconsistency, z-distances are simply both set to zero, which does not generate a bias.
    // otherwise distances are scaled such that height is weighted more.
    // if isGood>=3 && framesIncludedForScale < 0			===> START INTERVAL
    // if 18 <= framesIncludedForScale <= 36 AND isGood>=3	===> ADD INTERVAL, START INTERVAL
    // if framesIncludedForScale > 36						===> set framesIncludedForScale=-1 

    // include!

    // TODO: make shure filter is handled properly with permanent roll-forwards.
    if (goodCount_ >= 3) {
        ekf_.measurementCam(droneToWResult, timestamp);
        goodObservations_ += 1;
    } else {
        ekf_.clearUpToTime(timestamp);
    }

    // obtain results from EKF with updated position
    // TODO post EKF is not necessary IMO TooN::Vector<10> filterPosePostPTAM = ekf_.computeState(timestamp);

    const TooN::Vector<3> ptamPositionForScale(droneToWResult.get_translation() / scaleEstimation_.scale());
    const TooN::Vector<3> scalingFixpoint((scaleEstimation_.offsetMatrix().inverse() * droneToWResult).get_translation() / scaleEstimation_.scale());

    // if interval is started: add one step.
    const double includedTime = timestamp - ptamPositionForScaleTakenTimestamp_;
    if (framesIncludedForScaleXYZ_ >= 0) {
        framesIncludedForScaleXYZ_++;
    }

    // if interval is overdue: reset & dont add
    if (includedTime > 3.0) {
        framesIncludedForScaleXYZ_ = -1;
    }

    if (goodCount_ >= 3) {
        // filter stuff
        lastScaleEKFtimestamp_ = timestamp;

        if (includedTime >= 2.0 && framesIncludedForScaleXYZ_ > 1) // ADD! (if too many, was resetted before...)
        {
            TooN::Vector<3> diffPTAM = ptamPositionForScale - lastPtamPositionForScale_;
            bool zCorrupted, allCorrupted;

            TooN::Vector<3> diffIMU = evalNavQue(ptamPositionForScaleTakenTimestamp_, timestamp, &zCorrupted, &allCorrupted);

            /*
             * >>> Here was place for logging scale pairs (TODO write to an XVar)
             */


            if (!allCorrupted) {
                // filtering: z more weight, but only if not corrupted.
                double xyFactor = 0.05;
                double zFactor = zCorrupted ? 0 : 3;

                diffPTAM.slice<0, 2>() *= xyFactor;
                diffPTAM[2] *= zFactor;
                diffIMU.slice<0, 2>() *= xyFactor;
                diffIMU[2] *= zFactor;

                // hidden helluva calculations for scale estimate
                //scaleEstimation_.updateScale(diffPTAM, diffIMU, scalingFixpoint);
                ptamMapMaker_->currentScaleFactor = scaleEstimation_.scale();
                XCS_LOG_INFO("New scale: " << ptamMapMaker_->currentScaleFactor << "from diffs: " << diffPTAM << "; " << diffIMU);
            }
            framesIncludedForScaleXYZ_ = -1; // causing reset afterwards
        }

        if (framesIncludedForScaleXYZ_ == -1) // RESET!
        {
            framesIncludedForScaleXYZ_ = 0;
            lastPtamPositionForScale_ = ptamPositionForScale;
            //predIMUOnlyForScale_.resetPos();	// also resetting z corrupted flag etc. (NOT REquired as reset is done in eval)
            ptamPositionForScaleTakenTimestamp_ = timestamp;
        }
    }


    if (lockNextFrame_ && isGood) {
        scaleEstimation_.scalingFixpoint(scalingFixpoint);
        lockNextFrame_ = false;
        //filter->useScalingFixpoint = true; // this was commented out in original TUM

        XCS_LOG_INFO("locking scale fixpoint to " << scalingFixpoint[0] << " " << scalingFixpoint[1] << " " << scalingFixpoint[2]);
    }


    // ----------------------------- Take KF? -----------------------------------
    if (!mapLocked_ && isVeryGood && (forceKF_ || ptamMap_->vpKeyFrames.size() < maxKF_ || maxKF_ <= 1)) {
        ptamTracker_->TakeKF(forceKF_);
        forceKF_ = false;
    }

    // ---------------- save PTAM status for KI --------------------------------
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
     * No map and no logging so far.
     */
    //    // ----------------------------- update shallow map --------------------------
    //    if (!mapLocked_ && rand() % 5 == 0) { // TODO clarify purpose of this rand()
    //        pthread_mutex_lock(&shallowMapCS);
    //        mapPointsTransformed.clear();
    //        keyFramesTransformed.clear();
    //        for (unsigned int i = 0; i < ptamMap_->vpKeyFrames.size(); i++) {
    //            predConvert_.setPosSE3_globalToDrone(predConvert_.frontToDroneNT * ptamMap_->vpKeyFrames[i]->se3CfromW);
    //            TooN::Vector<6> CamPos = TooN::makeVector(predConvert_.x, predConvert_.y, predConvert_.z, predConvert_.roll, predConvert_.pitch, predConvert_.yaw);
    //            CamPos = filter->transformPTAMObservation(CamPos);
    //            predConvert_.setPosRPY(CamPos[0], CamPos[1], CamPos[2], CamPos[3], CamPos[4], CamPos[5]);
    //            keyFramesTransformed.push_back(predConvert_.droneToGlobal);
    //        }
    //        TooN::Vector<3> PTAMScales = filter->getCurrentScales();
    //        TooN::Vector<3> PTAMOffsets = filter->getCurrentOffsets().slice<0, 3>();
    //        for (unsigned int i = 0; i < ptamMap_->vpPoints.size(); i++) {
    //            TooN::Vector<3> pos = (ptamMap_->vpPoints)[i]->v3WorldPos;
    //            pos[0] *= PTAMScales[0];
    //            pos[1] *= PTAMScales[1];
    //            pos[2] *= PTAMScales[2];
    //            pos += PTAMOffsets;
    //            mapPointsTransformed.push_back(pos);
    //        }
    //
    //        // flush map keypoints
    //        if (flushMapKeypoints) {
    //            std::ofstream* fle = new std::ofstream();
    //            fle->open("pointcloud.txt");
    //
    //            for (unsigned int i = 0; i < mapPointsTransformed.size(); i++) {
    //                (*fle) << mapPointsTransformed[i][0] << " "
    //                        << mapPointsTransformed[i][1] << " "
    //                        << mapPointsTransformed[i][2] << std::endl;
    //            }
    //
    //            fle->flush();
    //            fle->close();
    //
    //            printf("FLUSHED %d KEYPOINTS to file pointcloud.txt\n\n", mapPointsTransformed.size());
    //
    //            flushMapKeypoints = false;
    //        }
    //
    //
    //        pthread_mutex_unlock(&shallowMapCS);
    //
    //    }
    //
    //
    //
    //    // ---------------------- output and render! ---------------------------
    //    ros::Duration timeALL = ros::Time::now() - startedFunc;
    //    if (isVeryGood) snprintf(charBuf, 1000, "\nQuality: best            ");
    //    else if (isGood) snprintf(charBuf, 1000, "\nQuality: good           ");
    //    else snprintf(charBuf, 1000, "\nQuality: lost                       ");
    //
    //    snprintf(charBuf + 20, 800, "scale: %.3f (acc: %.3f)                            ", filter->getCurrentScales()[0], (double) filter->getScaleAccuracy());
    //    snprintf(charBuf + 50, 800, "PTAM time: %i ms                            ", (int) (1000 * timeALL.toSec()));
    //    snprintf(charBuf + 68, 800, "(%i ms total)  ", (int) (1000 * timeALL.toSec()));
    //    if (mapLocked_) snprintf(charBuf + 83, 800, "m.l. ");
    //    else snprintf(charBuf + 83, 800, "     ");
    //    if (filter->allSyncLocked) snprintf(charBuf + 88, 800, "s.l. ");
    //    else snprintf(charBuf + 88, 800, "     ");
    //
    //
    //    msg += charBuf;
    //
    //    if (ptamMap_->IsGood()) {
    //        if (drawUI == UI_DEBUG) {
    //            snprintf(charBuf, 1000, "\nPTAM Diffs:              ");
    //            snprintf(charBuf + 13, 800, "x: %.3f                          ", diffs[0]);
    //            snprintf(charBuf + 23, 800, "y: %.3f                          ", diffs[1]);
    //            snprintf(charBuf + 33, 800, "z: %.3f                          ", diffs[2]);
    //            snprintf(charBuf + 43, 800, "r: %.2f                          ", diffs[3]);
    //            snprintf(charBuf + 53, 800, "p: %.2f                          ", diffs[4]);
    //            snprintf(charBuf + 63, 800, "y: %.2f", diffs[5]);
    //            msg += charBuf;
    //
    //
    //            snprintf(charBuf, 1000, "\nPTAM Pose:              ");
    //            snprintf(charBuf + 13, 800, "x: %.3f                          ", PTAMResultTransformed[0]);
    //            snprintf(charBuf + 23, 800, "y: %.3f                          ", PTAMResultTransformed[1]);
    //            snprintf(charBuf + 33, 800, "z: %.3f                          ", PTAMResultTransformed[2]);
    //            snprintf(charBuf + 43, 800, "r: %.2f                          ", PTAMResultTransformed[3]);
    //            snprintf(charBuf + 53, 800, "p: %.2f                          ", PTAMResultTransformed[4]);
    //            snprintf(charBuf + 63, 800, "y: %.2f", PTAMResultTransformed[5]);
    //            msg += charBuf;
    //
    //
    //            snprintf(charBuf, 1000, "\nPTAM WiggleDist:              ");
    //            snprintf(charBuf + 18, 800, "%.3f                          ", ptamMapMaker_->lastWiggleDist);
    //            snprintf(charBuf + 24, 800, "MetricDist: %.3f", ptamMapMaker_->lastMetricDist);
    //            msg += charBuf;
    //        }
    //    }
    //
    //    if (drawUI != UI_NONE) {
    //        // render grid
    //        predConvert_.setPosRPY(filterPosePostPTAM[0], filterPosePostPTAM[1], filterPosePostPTAM[2], filterPosePostPTAM[3], filterPosePostPTAM[4], filterPosePostPTAM[5]);
    //
    //        //renderGrid(predConvert->droneToFrontNT * predConvert->globaltoDrone);
    //        //renderGrid(PTAMResultSE3);
    //
    //
    //        // draw HUD
    //        //if(mod->getControlSystem()->isControlling())
    //        {
    //            glWindow_->SetupViewport();
    //            glWindow_->SetupVideoOrtho();
    //            glWindow_->SetupVideoRasterPosAndZoom();
    //
    //            //glDisable(GL_LINE_SMOOTH);
    //            glLineWidth(2);
    //            glBegin(GL_LINES);
    //            glColor3f(0, 0, 1);
    //
    //            glVertex2f(0, frameHeight / 2);
    //            glVertex2f(frameWidth, frameHeight / 2);
    //
    //            glVertex2f(frameWidth / 2, 0);
    //            glVertex2f(frameWidth / 2, frameHeight);
    //
    //            // 1m lines
    //            glVertex2f(0.25 * frameWidth, 0.48 * frameHeight);
    //            glVertex2f(0.25 * frameWidth, 0.52 * frameHeight);
    //            glVertex2f(0.75 * frameWidth, 0.48 * frameHeight);
    //            glVertex2f(0.75 * frameWidth, 0.52 * frameHeight);
    //            glVertex2f(0.48 * frameWidth, 0.25 * frameHeight);
    //            glVertex2f(0.52 * frameWidth, 0.25 * frameHeight);
    //            glVertex2f(0.48 * frameWidth, 0.75 * frameHeight);
    //            glVertex2f(0.52 * frameWidth, 0.75 * frameHeight);
    //
    //            glEnd();
    //        }
    //
    //
    //        glWindow_->DrawCaption(msg);
    //    }
    //
    //    lastPTAMResultRaw = PTAMResultSE3;
    //    // ------------------------ LOG --------------------------------------
    //    // log!
    //    if (node->logfilePTAM != NULL) {
    //        TooN::Vector<3> scales = filter->getCurrentScalesForLog();
    //        TooN::Vector<3> sums = TooN::makeVector(0, 0, 0);
    //        TooN::Vector<6> offsets = filter->getCurrentOffsets();
    //        pthread_mutex_lock(&(node->logPTAM_CS));
    //        // log:
    //        // - filterPosePrePTAM estimated for videoFrameTimestamp-delayVideo.
    //        // - PTAMResulttransformed estimated for videoFrameTimestamp-delayVideo. (using imu only for last step)
    //        // - predictedPoseSpeed estimated for lastNfoTimestamp+filter->delayControl	(actually predicting)
    //        // - predictedPoseSpeedATLASTNFO estimated for lastNfoTimestamp	(using imu only)
    //        if (node->logfilePTAM != NULL)
    //            (*(node->logfilePTAM)) << (isGood ? (isVeryGood ? 2 : 1) : 0) << " " <<
    //            (mimFrameTime_workingCopy - filter->delayVideo) << " " << filterPosePrePTAM[0] << " " << filterPosePrePTAM[1] << " " << filterPosePrePTAM[2] << " " << filterPosePrePTAM[3] << " " << filterPosePrePTAM[4] << " " << filterPosePrePTAM[5] << " " << filterPosePrePTAM[6] << " " << filterPosePrePTAM[7] << " " << filterPosePrePTAM[8] << " " << filterPosePrePTAM[9] << " " <<
    //                filterPosePostPTAM[0] << " " << filterPosePostPTAM[1] << " " << filterPosePostPTAM[2] << " " << filterPosePostPTAM[3] << " " << filterPosePostPTAM[4] << " " << filterPosePostPTAM[5] << " " << filterPosePostPTAM[6] << " " << filterPosePostPTAM[7] << " " << filterPosePostPTAM[8] << " " << filterPosePostPTAM[9] << " " <<
    //                PTAMResultTransformed[0] << " " << PTAMResultTransformed[1] << " " << PTAMResultTransformed[2] << " " << PTAMResultTransformed[3] << " " << PTAMResultTransformed[4] << " " << PTAMResultTransformed[5] << " " <<
    //                scales[0] << " " << scales[1] << " " << scales[2] << " " <<
    //                offsets[0] << " " << offsets[1] << " " << offsets[2] << " " << offsets[3] << " " << offsets[4] << " " << offsets[5] << " " <<
    //                sums[0] << " " << sums[1] << " " << sums[2] << " " <<
    //                PTAMResult[0] << " " << PTAMResult[1] << " " << PTAMResult[2] << " " << PTAMResult[3] << " " << PTAMResult[4] << " " << PTAMResult[5] << " " <<
    //                PTAMResultSE3TwistOrg[0] << " " << PTAMResultSE3TwistOrg[1] << " " << PTAMResultSE3TwistOrg[2] << " " << PTAMResultSE3TwistOrg[3] << " " << PTAMResultSE3TwistOrg[4] << " " << PTAMResultSE3TwistOrg[5] << " " <<
    //                videoFramePing << " " << mimFrameTimeRos_workingCopy << " " << frameNo_ << std::endl;
    //
    //        pthread_mutex_unlock(&(node->logPTAM_CS));
    //    }


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
                // pop
                //navInfoQueue.pop_front();
            } else {
                break;
            }
        }

    }
    //printf("QueEval: before: %i; skipped: %i, used: %i, left: %i\n", totSize, skipped, used, navInfoQueue.size());
    predIMUOnlyForScale_.z -= firstZ; // make height to height-diff

    *zCorrupted = predIMUOnlyForScale_.zCorrupted;
    *allCorrupted = std::abs(firstAdded - from) + std::abs(lastAdded - to) > 0.08; // 80 ms //TODO check this abs'es are correct

    //    if (*allCorrupted)
    //        printf("scalePackage corrupted (imu data gap for %ims)\n", abs(firstAdded - (int) from) + abs(lastAdded - (int) to));
    //    else if (*zCorrupted)
    //        printf("scalePackage z corrupted (jump in meters: %.3f)!\n", predIMUOnlyForScale_.zCorruptedJump);
    //
    //    printf("first: %f (%f); last: %f (%f)=> diff: %f (z alt diff: %f)\n",
    //            sum_first / num_first,
    //            num_first,
    //            sum_last / num_last,
    //            num_last,
    //            sum_last / num_last - sum_first / num_first,
    //            predIMUOnlyForScale_.z
    //            );

    return TooN::makeVector(predIMUOnlyForScale_.x, predIMUOnlyForScale_.y, predIMUOnlyForScale_.z);
}

void Ptam::resetPtam() {
    // (re) create all PTAM instances
    ptamCamera_ = ATANCameraPtr(new ATANCamera(cameraParameters_));
    ptamMap_ = MapPtr(new Map());
    ptamMapMaker_ = MapMakerPtr(new MapMaker(*ptamMap_, *ptamCamera_));
    ptamTracker_ = TrackerPtr(new Tracker(CVD::ImageRef(FRAME_WIDTH, FRAME_HEIGHT), *ptamCamera_, *ptamMap_, *ptamMapMaker_));

    // TODO configuration
    ptamMapMaker_->minKFDist = 0;
    ptamMapMaker_->minKFWiggleDist = 0;
    ptamTracker_->minKFTimeDist = 0;

    predIMUOnlyForScale_.resetPos();

    goodCount_ = 0;
    forceKF_ = false;
    goodObservations_ = 0;
    lockNextFrame_ = false;

    resetPtamRequested_ = false;
}