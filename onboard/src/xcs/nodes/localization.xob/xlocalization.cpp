#include "xlocalization.hpp"
#include <limits>
#include <xcs/logging.hpp>
#include <xcs/xcs_fce.hpp>

using namespace xcs::nodes;
using namespace xcs::nodes::localization;

const double XLocalization::IMU_DELAY = 0.040; // 40ms
const double XLocalization::FLY_CONTROL_SEND_TIME = 0.075; // 75ms
const double XLocalization::CAM_DELAY = 0.100; // 

void XLocalization::onChangeVelocity(xcs::CartesianVector measuredVelocity) {
    lastMeasurement_.velocity.x = measuredVelocity.y;
    lastMeasurement_.velocity.y = measuredVelocity.x;
}

void XLocalization::onChangeRotation(xcs::EulerianVector measuredAnglesRotation) {
    lastMeasurement_.angularRotationPsi = xcs::normAngle(measuredAnglesRotation.psi - lastMeasurement_.angles.psi);
    lastMeasurement_.angles = measuredAnglesRotation;
}

void XLocalization::onChangeAltitude(double altitude) {
    lastMeasurement_.velocity.z = (altitude - lastMeasurement_.altitude);
    lastMeasurement_.altitude = altitude;
}

void XLocalization::onChangeTimeImu(double timeImu) {
    if (imuTimeShift_ == std::numeric_limits<double>::max()) {
        imuTimeShift_ = timeImu - timeFromStart();
    }

    double actualTime = timeImu - imuTimeShift_ - IMU_DELAY;
    lastMeasurement_.velocity.z /= (actualTime - lastMeasurementTime_);
    lastMeasurement_.angularRotationPsi /= (actualTime - lastMeasurementTime_);
    ekf_.measurementImu(lastMeasurement_, actualTime); // TODO: compute measurement time delay
    //printf("Measurement time %f actual time %f \n", actualTime, timeFromStart());

    // update actual position of drone
    DroneState state = ekf_.computeState(timeFromStart());
    position = state.position;
    velocity = state.velocity;
    rotation = state.angles;
}

void XLocalization::onChangePosition(xcs::CartesianVector measuredPosition) {
    lastCameraMeasurement_.position = measuredPosition;
}

void XLocalization::onChangeAngles(xcs::EulerianVector measuredAngles) {
    lastCameraMeasurement_.angles = measuredAngles;
}

void XLocalization::onChangeTimeCam(double timeCam) {
    double time = timeCam - imuTimeShift_ - CAM_DELAY;
    ekf_.measurementCam(lastCameraMeasurement_, time);

    // update actual position of drone
    DroneState state = ekf_.computeState(timeFromStart());
    position = state.position;
    velocity = state.velocity;
    rotation = state.angles;
}

void XLocalization::onChangeClearTime(double time) {
    double droneTime = time - imuTimeShift_ - CAM_DELAY;
    ekf_.clearUpToTime(droneTime);
}

void XLocalization::onChangeVideo(urbi::UImage bwImage) {
    XCS_LOG_INFO("new video frame");
//    frameNo_ += 1;
//
//    /*
//     * TODO reset PTAM
//     */
//    if (resetPtamRequested_) {
//        //resetPTAMRequested_ = false;
//    }
//
//    /*
//     * Convert image to grayscale and to correct format for CVD
//     */
//    urbi::UImage bwImage;
//    bwImage.imageFormat = urbi::IMAGE_GREY8;
//    bwImage.data = nullptr;
//    bwImage.size = 0;
//    bwImage.width = 0;
//    bwImage.height = 0;
//    urbi::convert(image, bwImage);
//
//    if (frame_.size().x != bwImage.width || frame_.size().y != bwImage.height) {
//        frame_.resize(CVD::ImageRef(bwImage.width, bwImage.height));
//    }
//
//    memcpy(frame_.data(), bwImage.data, bwImage.width * bwImage.height);

}

void XLocalization::onChangeFlyControl(xcs::FlyControl flyControl) {
    //printf("FlyControl time %f\n", timeFromStart());
    ekf_.flyControl(flyControl, timeFromStart() + FLY_CONTROL_SEND_TIME); // TODO: compute flyControl delay
}

//================ public functions ==================

XLocalization::XLocalization(const std::string &name) :
  XObject(name),
  measuredVelocity("VELOCITY"),
  measuredAnglesRotation("ROTATION"),
  measuredAltitude("ALTITUDE"),
  timeImu("TIME_LOC"),
  measuredPosition("POSITION_ABS"),
  measuredAngles("ROTATION"),
  timeCam("TIME_LOC"),
  clearTime("TIME_LOC"),
  video("FRONT_CAMERA"),
  flyControl("FLY_CONTROL"),
  position("POSITION_ABS"),
  velocity("VELOCITY_ABS"),
  rotation("ROTATION") {
    startTime_ = clock_.now();
    lastMeasurementTime_ = 0;
    imuTimeShift_ = std::numeric_limits<double>::max();

    XBindVarF(measuredVelocity, &XLocalization::onChangeVelocity);
    XBindVarF(measuredAnglesRotation, &XLocalization::onChangeRotation);
    XBindVarF(measuredAltitude, &XLocalization::onChangeAltitude);
    XBindVarF(timeImu, &XLocalization::onChangeTimeImu);

    XBindVarF(measuredPosition, &XLocalization::onChangePosition);
    XBindVarF(measuredAngles, &XLocalization::onChangeAngles);
    XBindVarF(timeCam, &XLocalization::onChangeTimeCam);
    XBindVarF(clearTime, &XLocalization::onChangeClearTime);

    XBindVarF(video, &XLocalization::onChangeVideo);
    
    XBindVarF(flyControl, &XLocalization::onChangeFlyControl);

    XBindVar(position);
    XBindVar(velocity);
    XBindVar(rotation);
}

XStart(XLocalization);