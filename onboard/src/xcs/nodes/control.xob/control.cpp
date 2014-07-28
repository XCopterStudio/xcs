#include "control.hpp"

#include <xcs/xcs_fce.hpp>

using namespace xcs;
using namespace xcs::nodes::control;

const double Control::MAX_VALUE = 1.0;

// ================ public functions ===============
Control::Control() : 
vxPID_(0.1, 0, 0),
vyPID_(0.1, 0, 0), 
vzPID_(0.1, 0, 0),
psiPID_(0.1, 0, 0) {
}

void Control::velocity(const CartesianVector &velocity){
    velocity_ = velocity;
}

void Control::rotation(const EulerianVector &rotation){
    rotation_ = rotation;
}

void Control::desireVelocity(const xcs::SpeedControl &speedControl){
    desireVelocity_ = speedControl;
    desireVelocity_.psi = xcs::normAngle(desireVelocity_.psi);
}

xcs::FlyControl Control::computeControl(){

    double vx = vxPID_.getCorrection(velocity_.x, desireVelocity_.vx);
    double vy = vyPID_.getCorrection(velocity_.y, desireVelocity_.vy);
    double angleError = xcs::normAngle(desireVelocity_.psi - rotation_.psi);

    //printf("Control: actual speed [%f,%f,%f,%f] \n", velocity_.x, velocity_.y, velocity_.z, rotation_.psi);
    //printf("Control: Control error vx: %f vy: %f vz: %f \n", vx,vy,velocity_.z);

    xcs::FlyControl flyControl;
    flyControl.roll =  valueInRange<double>(cos(rotation_.psi)*vx + sin(rotation_.psi)*vy, MAX_VALUE);
    flyControl.pitch = -valueInRange<double>(-sin(rotation_.psi)*vx + cos(rotation_.psi)*vy, MAX_VALUE);
    flyControl.yaw = valueInRange<double>(psiPID_.getCorrection(angleError), MAX_VALUE);
    flyControl.gaz = valueInRange<double>(vzPID_.getCorrection(velocity_.z, desireVelocity_.vz), MAX_VALUE);

    return flyControl;
}