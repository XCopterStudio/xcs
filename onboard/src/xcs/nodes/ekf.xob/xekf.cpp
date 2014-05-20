#include "xekf.hpp"
#include <limits>
#include <xcs/logging.hpp>
#include <xcs/xcs_fce.hpp>

using namespace xcs::nodes;
using namespace xcs::nodes::ekf;

const double XEkf::IMU_DELAY = 0.040; // 40ms
const double XEkf::FLY_CONTROL_SEND_TIME = 0.075; // 75ms
const double XEkf::CAM_DELAY = 0.100; // 

void XEkf::onChangeVelocity(xcs::CartesianVector measuredVelocity){
    lastMeasurement_.velocity.x = measuredVelocity.y;
    lastMeasurement_.velocity.y = measuredVelocity.x;
}

void XEkf::onChangeRotation(xcs::EulerianVector measuredAnglesRotation){
    lastMeasurement_.angularRotationPsi = xcs::normAngle(measuredAnglesRotation.psi - lastMeasurement_.angles.psi);
    lastMeasurement_.angles = measuredAnglesRotation;
}

void XEkf::onChangeAltitude(double altitude){
    lastMeasurement_.velocity.z = (altitude - lastMeasurement_.altitude);
    lastMeasurement_.altitude = altitude;
}

void XEkf::onChangeTimeImu(double timeImu){
    if (imuTimeShift_ == std::numeric_limits<double>::max()){
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

void XEkf::onChangePosition(xcs::CartesianVector measuredPosition){
    lastCameraMeasurement_.position = measuredPosition;
}

void XEkf::onChangeAngles(xcs::EulerianVector measuredAngles){
    lastCameraMeasurement_.angles = measuredAngles;
}

void XEkf::onChangeTimeCam(double timeCam){
    double time = timeCam - imuTimeShift_ - CAM_DELAY;
    ekf_.measurementCam(lastCameraMeasurement_, time);

    // update actual position of drone
    DroneState state = ekf_.computeState(timeFromStart());
    position = state.position;
    velocity = state.velocity;
    rotation = state.angles;
}

void XEkf::onChangeClearTime(double time){
    double droneTime = time - imuTimeShift_ - CAM_DELAY;
    ekf_.clearUpToTime(droneTime);
}

void XEkf::onChangeFlyControl(xcs::FlyControl flyControl){
    //printf("FlyControl time %f\n", timeFromStart());
    ekf_.flyControl(flyControl, timeFromStart() + FLY_CONTROL_SEND_TIME); // TODO: compute flyControl delay
}

//================ public functions ==================

XEkf::XEkf(const std::string &name) : 
XObject(name), 
measuredVelocity("VELOCITY"),
measuredAnglesRotation("ROTATION"),
measuredAltitude("ALTITUDE"),
timeImu("TIME_LOC"),
measuredPosition("POSITION_ABS"),
measuredAngles("ROTATION"),
timeCam("TIME_LOC"),
clearTime("TIME_LOC"),
flyControl("FLY_CONTROL"), 
position("POSITION_ABS"),
velocity("VELOCITY_ABS"),
rotation("ROTATION"){
    startTime_ = clock_.now();
    lastMeasurementTime_ = 0;
    imuTimeShift_ = std::numeric_limits<double>::max();

    XBindVarF(measuredVelocity,&XEkf::onChangeVelocity);
    XBindVarF(measuredAnglesRotation,&XEkf::onChangeRotation);
    XBindVarF(measuredAltitude,&XEkf::onChangeAltitude);
    XBindVarF(timeImu,&XEkf::onChangeTimeImu);

    XBindVarF(measuredPosition,&XEkf::onChangePosition);
    XBindVarF(measuredAngles,&XEkf::onChangeAngles);
    XBindVarF(timeCam,&XEkf::onChangeTimeCam);
    XBindVarF(clearTime,&XEkf::onChangeClearTime);

    XBindVarF(flyControl,&XEkf::onChangeFlyControl);

    XBindVar(position);
    XBindVar(velocity);
    XBindVar(rotation);
}

XStart(XEkf);