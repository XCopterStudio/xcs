#include "xekf.hpp"
#include <limits>
#include <xcs/logging.hpp>
#include <xcs/xcs_fce.hpp>

using namespace xcs::nodes;

const double XEkf::IMU_DELAY = 0.060; // 60ms
const double XEkf::FLY_CONTROL_SEND_TIME = 0.040; // 40ms

void XEkf::onChangeVelocity(xcs::CartesianVector measuredVelocity){
    lastMeasurement_.velocity = measuredVelocity;
}

void XEkf::onChangeRotation(xcs::EulerianVector measuredAnglesRotation){
    lastMeasurement_.angularRotationPsi = xcs::normAngle(measuredAnglesRotation.psi - lastMeasurement_.angles.psi);
    lastMeasurement_.angles = measuredAnglesRotation;
}

void XEkf::onChangeAltitude(double altitude){
    lastMeasurement_.velocity.z = altitude - lastAltitude_;
    lastMeasurement_.altitude = altitude;
}

void XEkf::onChangeTimeImu(double timeImu){
    if (imuTimeShift_ == std::numeric_limits<double>::max()){
        imuTimeShift_ = timeImu - timeFromStart();
    }

    double actualTime = timeImu - imuTimeShift_ - IMU_DELAY;
    lastMeasurement_.angularRotationPsi /= (actualTime - lastMeasurementTime_);
    ekf_.measurement(lastMeasurement_, actualTime); // TODO: compute measurement time delay
}

void XEkf::onChangeFlyControl(xcs::FlyControl flyControl){
    ekf_.flyControl(flyControl, timeFromStart() + FLY_CONTROL_SEND_TIME); // TODO: compute flyControl delay
}

//================ public functions ==================

XEkf::XEkf(const std::string &name) : 
XObject(name), 
measuredVelocity("VELOCITY"),
measuredAnglesRotation("ROTATION"),
measuredAltitude("ALTITUDE"),
timeImu("TIME_LOC"),
flyControl("FLY_CONTROL"), 
position("POSITION_ABS"),
velocity("VELOCITY_ABS"),
rotation("ROTATION"){
    startTime_ = clock_.now();
    lastAltitude_ = 0;
    lastMeasurementTime_ = 0;
    imuTimeShift_ = std::numeric_limits<double>::max();

    XBindVarF(measuredVelocity,&XEkf::onChangeVelocity);
    XBindVarF(measuredAnglesRotation,&XEkf::onChangeRotation);
    XBindVarF(measuredAltitude,&XEkf::onChangeAltitude);
    XBindVarF(timeImu,&XEkf::onChangeTimeImu);
    XBindVarF(flyControl,&XEkf::onChangeFlyControl);

    XBindVar(position);
    XBindVar(velocity);
    XBindVar(rotation);
}

XStart(XEkf);