#include "x_ptam.hpp"

#include "tum/tum_utils.hpp"

#include <TooN/TooN.h>

#include <xcs/logging.hpp>


using namespace std;
using namespace xcs::nodes;
using namespace xcs::nodes::ptam;

const int XPtam::FRAME_WIDTH = 360;
const int XPtam::FRAME_HEIGHT = 640;

XPtam::XPtam(const std::string &name) :
  XObject(name),
  video("FRONT_CAMERA") {
    XBindVarF(video, &XPtam::onChangeVideo);
    XBindFunction(XPtam, init);
}

XPtam::~XPtam() {

}

void XPtam::init() {
    // predictors

    // (G)UI type

    // KF parameters




    // TODO load camera parameters from configuration (these are for AR Drone Parrot 2.0)
    TooN::Vector<5> cameraParameters;
    cameraParameters[0] = 0.771557;
    cameraParameters[1] = 1.368560;
    cameraParameters[2] = 0.552779;
    cameraParameters[3] = 0.444056;
    cameraParameters[4] = 1.156010;

    // TODO wrap this inside a reset method (and allow user to reset whole PTAM)
    ptamCamera_ = ATANCameraPtr(new ATANCamera(cameraParameters));
    ptamMap_ = MapPtr(new Map());
    ptamMapMaker_ = MapMakerPtr(new MapMaker(*ptamMap_, *ptamCamera_));
    ptamTracker_ = TrackerPtr(new Tracker(CVD::ImageRef(FRAME_WIDTH, FRAME_HEIGHT), *ptamCamera_, *ptamMap_, *ptamMapMaker_)); // TODO video resolution (metadata??
    
    glWindow_ = new GLWindow2(CVD::ImageRef(FRAME_WIDTH, FRAME_HEIGHT), "PTAM Drone Camera Feed", this);
}

void XPtam::onChangeVideo(::urbi::UImage image) {
    XCS_LOG_INFO("new video frame");


    TooN::Vector<10> filterPosePrePTAM; // TODO get state from EKF in correct time
    //= filter->getPoseAtAsVec(mimFrameTime_workingCopy - filter->delayVideo, true);

    // ------------------------ do PTAM -------------------------
    glWindow_->SetupViewport();
    glWindow_->SetupVideoOrtho();
    glWindow_->SetupVideoRasterPosAndZoom();



    // 1. transform with filter
    TooN::Vector<6> PTAMPoseGuess = TumUtils::backTransformPTAMObservation(filterPosePrePTAM.slice<0, 6>());
    // 2. convert to se3
    predConvert_->setPosRPY(PTAMPoseGuess[0], PTAMPoseGuess[1], PTAMPoseGuess[2], PTAMPoseGuess[3], PTAMPoseGuess[4], PTAMPoseGuess[5]);
    // 3. multiply with rotation matrix	
    TooN::SE3<> PTAMPoseGuessSE3 = predConvert_->droneToFrontNT * predConvert_->globaltoDrone;


//    // set
//    ptamTracker_->setPredictedCamFromW(PTAMPoseGuessSE3);
//    //ptamTracker_->setLastFrameLost((isGoodCount < -10), (videoFrameID%2 != 0));
//    ptamTracker_->setLastFrameLost((isGoodCount < -20), (mimFrameSEQ_workingCopy % 3 == 0));
//
//    // track
//    ros::Time startedPTAM = ros::Time::now();
//    ptamTracker_->TrackFrame(mimFrameBW_workingCopy, true);
//    TooN::SE3<> PTAMResultSE3 = ptamTracker_->GetCurrentPose();
//    lastPTAMMessage = msg = ptamTracker_->GetMessageForUser();
//    ros::Duration timePTAM = ros::Time::now() - startedPTAM;
//
//    TooN::Vector<6> PTAMResultSE3TwistOrg = PTAMResultSE3.ln();
//
//    node->publishTf(ptamTracker_->GetCurrentPose(), mimFrameTimeRos_workingCopy, mimFrameSEQ_workingCopy, "cam_front");
//
//
//    // 1. multiply from left by frontToDroneNT.
//    // 2. convert to xyz,rpy
//    predConvert_->setPosSE3_globalToDrone(predConvert_->frontToDroneNT * PTAMResultSE3);
//    TooN::Vector<6> PTAMResult = TooN::makeVector(predConvert_->x, predConvert_->y, predConvert_->z, predConvert_->roll, predConvert_->pitch, predConvert_->yaw);
//
//    // 3. transform with filter
//    TooN::Vector<6> PTAMResultTransformed = filter->transformPTAMObservation(PTAMResult);
//
//
//
//
//    // init failed?
//    if (ptamTracker_->lastStepResult == ptamTracker_->I_FAILED) {
//        ROS_INFO("initializing PTAM failed, resetting!");
//        resetPTAMRequested = true;
//    }
//    if (ptamTracker_->lastStepResult == ptamTracker_->I_SECOND) {
//        PTAMInitializedClock = getMS();
//        filter->setCurrentScales(TooN::makeVector(mpMapMaker->initialScaleFactor * 1.2, mpMapMaker->initialScaleFactor * 1.2, mpMapMaker->initialScaleFactor * 1.2));
//        mpMapMaker->currentScaleFactor = filter->getCurrentScales()[0];
//        ROS_INFO("PTAM initialized!");
//        ROS_INFO("initial scale: %f\n", mpMapMaker->initialScaleFactor * 1.2);
//        node->publishCommand("u l PTAM initialized (took second KF)");
//        framesIncludedForScaleXYZ = -1;
//        lockNextFrame = true;
//        imuOnlyPred->resetPos();
//    }
//    if (ptamTracker_->lastStepResult == ptamTracker_->I_FIRST) {
//        node->publishCommand("u l PTAM initialization started (took first KF)");
//    }
//
//
//
//
//
//
//    // --------------------------- assess result ------------------------------
//    bool isGood = true;
//    bool isVeryGood = true;
//    // calculate absolute differences.
//    TooN::Vector<6> diffs = PTAMResultTransformed - filterPosePrePTAM.slice<0, 6>();
//    for (int i = 0; 1 < 1; i++) diffs[i] = abs(diffs[i]);
//
//
//    if (filter->getNumGoodPTAMObservations() < 10 && mpMap->IsGood()) {
//        isGood = true;
//        isVeryGood = false;
//    } else if (ptamTracker_->lastStepResult == ptamTracker_->I_FIRST ||
//            ptamTracker_->lastStepResult == ptamTracker_->I_SECOND ||
//            ptamTracker_->lastStepResult == ptamTracker_->I_FAILED ||
//            ptamTracker_->lastStepResult == ptamTracker_->T_LOST ||
//            ptamTracker_->lastStepResult == ptamTracker_->NOT_TRACKING ||
//            ptamTracker_->lastStepResult == ptamTracker_->INITIALIZING)
//        isGood = isVeryGood = false;
//    else {
//        // some chewy heuristic when to add and when not to.
//        bool dodgy = ptamTracker_->lastStepResult == ptamTracker_->T_DODGY ||
//                ptamTracker_->lastStepResult == ptamTracker_->T_RECOVERED_DODGY;
//
//        // if yaw difference too big: something certainly is wrong.
//        // maximum difference is 5 + 2*(number of seconds since PTAM observation).
//        double maxYawDiff = 10.0 + (getMS() - lastGoodYawClock)*0.002;
//        if (maxYawDiff > 20) maxYawDiff = 1000;
//        if (false && diffs[5] > maxYawDiff)
//            isGood = false;
//
//        if (diffs[5] < 10)
//            lastGoodYawClock = getMS();
//
//        if (diffs[5] > 4.0)
//            isVeryGood = false;
//
//        // if rp difference too big: something certainly is wrong.
//        if (diffs[3] > 20 || diffs[4] > 20)
//            isGood = false;
//
//        if (diffs[3] > 3 || diffs[4] > 3 || dodgy)
//            isVeryGood = false;
//    }
//
//    if (isGood) {
//        if (isGoodCount < 0) isGoodCount = 0;
//        isGoodCount++;
//    } else {
//        if (isGoodCount > 0) isGoodCount = 0;
//        isGoodCount--;
//
//        if (ptamTracker_->lastStepResult == ptamTracker_->T_RECOVERED_DODGY)
//            isGoodCount = std::max(isGoodCount, -2);
//        if (ptamTracker_->lastStepResult == ptamTracker_->T_RECOVERED_GOOD)
//            isGoodCount = std::max(isGoodCount, -5);
//
//    }
//
//    TooN::Vector<10> filterPosePostPTAM;
//    // --------------------------- scale estimation & update filter (REDONE) -----------------------------
//    // interval length is always between 1s and 2s, to enshure approx. same variances.
//    // if interval contained height inconsistency, z-distances are simply both set to zero, which does not generate a bias.
//    // otherwise distances are scaled such that height is weighted more.
//    // if isGood>=3 && framesIncludedForScale < 0			===> START INTERVAL
//    // if 18 <= framesIncludedForScale <= 36 AND isGood>=3	===> ADD INTERVAL, START INTERVAL
//    // if framesIncludedForScale > 36						===> set framesIncludedForScale=-1 
//
//    // include!
//
//    // TODO: make shure filter is handled properly with permanent roll-forwards.
//    pthread_mutex_lock(&filter->filter_CS);
//    if (filter->usePTAM && isGoodCount >= 3) {
//        filter->addPTAMObservation(PTAMResult, mimFrameTime_workingCopy - filter->delayVideo);
//    } else
//        filter->addFakePTAMObservation(mimFrameTime_workingCopy - filter->delayVideo);
//
//    filterPosePostPTAM = filter->getCurrentPoseSpeedAsVec();
//    pthread_mutex_unlock(&filter->filter_CS);
//
//    TooN::Vector<6> filterPosePostPTAMBackTransformed = filter->backTransformPTAMObservation(filterPosePostPTAM.slice<0, 6>());
//
//
//    // if interval is started: add one step.
//    int includedTime = mimFrameTime_workingCopy - ptamPositionForScaleTakenTimestamp;
//    if (framesIncludedForScaleXYZ >= 0) framesIncludedForScaleXYZ++;
//
//    // if interval is overdue: reset & dont add
//    if (includedTime > 3000) {
//        framesIncludedForScaleXYZ = -1;
//    }
//
//    if (isGoodCount >= 3) {
//        // filter stuff
//        lastScaleEKFtimestamp = mimFrameTime_workingCopy;
//
//        if (includedTime >= 2000 && framesIncludedForScaleXYZ > 1) // ADD! (if too many, was resetted before...)
//        {
//            TooN::Vector<3> diffPTAM = filterPosePostPTAMBackTransformed.slice<0, 3>() - PTAMPositionForScale;
//            bool zCorrupted, allCorrupted;
//            float pressureStart = 0, pressureEnd = 0;
//            TooN::Vector<3> diffIMU = evalNavQue(ptamPositionForScaleTakenTimestamp - filter->delayVideo + filter->delayXYZ, mimFrameTime_workingCopy - filter->delayVideo + filter->delayXYZ, &zCorrupted, &allCorrupted, &pressureStart, &pressureEnd);
//
//            pthread_mutex_lock(&logScalePairs_CS);
//            if (logfileScalePairs != 0)
//                (*logfileScalePairs) <<
//                pressureStart << " " <<
//                    pressureEnd << " " <<
//                    diffIMU[2] << " " <<
//                    diffPTAM[2] << std::endl;
//            pthread_mutex_unlock(&logScalePairs_CS);
//
//
//            if (!allCorrupted) {
//                // filtering: z more weight, but only if not corrupted.
//                double xyFactor = 0.05;
//                double zFactor = zCorrupted ? 0 : 3;
//
//                diffPTAM.slice<0, 2>() *= xyFactor;
//                diffPTAM[2] *= zFactor;
//                diffIMU.slice<0, 2>() *= xyFactor;
//                diffIMU[2] *= zFactor;
//
//                filter->updateScaleXYZ(diffPTAM, diffIMU, PTAMResult.slice<0, 3>());
//                mpMapMaker->currentScaleFactor = filter->getCurrentScales()[0];
//            }
//            framesIncludedForScaleXYZ = -1; // causing reset afterwards
//        }
//
//        if (framesIncludedForScaleXYZ == -1) // RESET!
//        {
//            framesIncludedForScaleXYZ = 0;
//            PTAMPositionForScale = filterPosePostPTAMBackTransformed.slice<0, 3>();
//            //predIMUOnlyForScale->resetPos();	// also resetting z corrupted flag etc. (NOT REquired as reset is done in eval)
//            ptamPositionForScaleTakenTimestamp = mimFrameTime_workingCopy;
//        }
//    }
//
//
//    if (lockNextFrame && isGood) {
//        filter->scalingFixpoint = PTAMResult.slice<0, 3>();
//        lockNextFrame = false;
//        //filter->useScalingFixpoint = true;
//
//        snprintf(charBuf, 500, "locking scale fixpoint to %.3f %.3f %.3f", PTAMResultTransformed[0], PTAMResultTransformed[1], PTAMResultTransformed[2]);
//        ROS_INFO(charBuf);
//        node->publishCommand(std::string("u l ") + charBuf);
//    }
//
//
//    // ----------------------------- Take KF? -----------------------------------
//    if (!mapLocked && isVeryGood && (forceKF || mpMap->vpKeyFrames.size() < maxKF || maxKF <= 1)) {
//        ptamTracker_->TakeKF(forceKF);
//        forceKF = false;
//    }
//
//    // ---------------- save PTAM status for KI --------------------------------
//    if (ptamTracker_->lastStepResult == ptamTracker_->NOT_TRACKING)
//        PTAMStatus = PTAM_IDLE;
//    else if (ptamTracker_->lastStepResult == ptamTracker_->I_FIRST ||
//            ptamTracker_->lastStepResult == ptamTracker_->I_SECOND ||
//            ptamTracker_->lastStepResult == ptamTracker_->T_TOOK_KF)
//        PTAMStatus = PTAM_TOOKKF;
//    else if (ptamTracker_->lastStepResult == ptamTracker_->INITIALIZING)
//        PTAMStatus = PTAM_INITIALIZING;
//    else if (isVeryGood)
//        PTAMStatus = PTAM_BEST;
//    else if (isGood)
//        PTAMStatus = PTAM_GOOD;
//    else if (ptamTracker_->lastStepResult == ptamTracker_->T_DODGY ||
//            ptamTracker_->lastStepResult == ptamTracker_->T_GOOD)
//        PTAMStatus = PTAM_FALSEPOSITIVE;
//    else
//        PTAMStatus = PTAM_LOST;
//
//
//    // ----------------------------- update shallow map --------------------------
//    if (!mapLocked && rand() % 5 == 0) {
//        pthread_mutex_lock(&shallowMapCS);
//        mapPointsTransformed.clear();
//        keyFramesTransformed.clear();
//        for (unsigned int i = 0; i < mpMap->vpKeyFrames.size(); i++) {
//            predConvert_->setPosSE3_globalToDrone(predConvert_->frontToDroneNT * mpMap->vpKeyFrames[i]->se3CfromW);
//            TooN::Vector<6> CamPos = TooN::makeVector(predConvert_->x, predConvert_->y, predConvert_->z, predConvert_->roll, predConvert_->pitch, predConvert_->yaw);
//            CamPos = filter->transformPTAMObservation(CamPos);
//            predConvert_->setPosRPY(CamPos[0], CamPos[1], CamPos[2], CamPos[3], CamPos[4], CamPos[5]);
//            keyFramesTransformed.push_back(predConvert_->droneToGlobal);
//        }
//        TooN::Vector<3> PTAMScales = filter->getCurrentScales();
//        TooN::Vector<3> PTAMOffsets = filter->getCurrentOffsets().slice<0, 3>();
//        for (unsigned int i = 0; i < mpMap->vpPoints.size(); i++) {
//            TooN::Vector<3> pos = (mpMap->vpPoints)[i]->v3WorldPos;
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
//    if (mapLocked) snprintf(charBuf + 83, 800, "m.l. ");
//    else snprintf(charBuf + 83, 800, "     ");
//    if (filter->allSyncLocked) snprintf(charBuf + 88, 800, "s.l. ");
//    else snprintf(charBuf + 88, 800, "     ");
//
//
//    msg += charBuf;
//
//    if (mpMap->IsGood()) {
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
//            snprintf(charBuf + 18, 800, "%.3f                          ", mpMapMaker->lastWiggleDist);
//            snprintf(charBuf + 24, 800, "MetricDist: %.3f", mpMapMaker->lastMetricDist);
//            msg += charBuf;
//        }
//    }
//
//    if (drawUI != UI_NONE) {
//        // render grid
//        predConvert_->setPosRPY(filterPosePostPTAM[0], filterPosePostPTAM[1], filterPosePostPTAM[2], filterPosePostPTAM[3], filterPosePostPTAM[4], filterPosePostPTAM[5]);
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
//                videoFramePing << " " << mimFrameTimeRos_workingCopy << " " << mimFrameSEQ_workingCopy << std::endl;
//
//        pthread_mutex_unlock(&(node->logPTAM_CS));
//    }

    glWindow_->swap_buffers();
    glWindow_->HandlePendingEvents();
}

XStart(XPtam);