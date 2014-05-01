#include "control.hpp"

#include <xcs/xcs_fce.hpp>

using namespace xcs;
using namespace xcs::nodes::control;

const double Control::MAX_VALUE = 1.0;

xcs::FlyControl Control::computeControl(){

    double vx = vxPID_.getCorrection(velocity_.x, desireSpeed_.vx);
    double vy = vxPID_.getCorrection(velocity_.y, desireSpeed_.vy);

    xcs::FlyControl flyControl;
    flyControl.roll = valueInRange<double>(cos(rotation_.psi)*vx + sin(rotation_.psi)*vy, MAX_VALUE);
    flyControl.pitch = valueInRange<double>(-sin(rotation_.psi)*vx - cos(rotation_.psi)*vy, MAX_VALUE);
    flyControl.yaw = valueInRange<double>(psiPID_.getCorrection(rotation_.psi, desireSpeed_.psi), MAX_VALUE);
    flyControl.gaz = valueInRange<double>(vzPID_.getCorrection(velocity_.z,desireSpeed_.vz), MAX_VALUE);

    return flyControl;
}

// ================ public functions ===============
Control::Control(){
}

void Control::updateVelocity(const CartesianVector &velocity){
    velocity_ = velocity;
}

FlyControl Control::updateRotation(const EulerianVector &rotation){
    rotation_ = rotation;
    return computeControl();
}

void Control::updateSpeedControl(const xcs::SpeedControl &speedControl){
    desireSpeed_ = speedControl;
}