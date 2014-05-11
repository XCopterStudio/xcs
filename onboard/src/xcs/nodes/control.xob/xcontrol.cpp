#include "xcontrol.hpp"

using namespace xcs;
using namespace xcs::nodes::control;

void XControl::onChangeVelocity(xcs::CartesianVector velocity){
    control_.velocity(velocity);
}

void XControl::onChangeRotation(xcs::EulerianVector rotation){
    control_.rotation(rotation);
    flyControl = control_.computeControl();
}

void XControl::onChangeDesireSpeed(xcs::SpeedControl desireSpeed){
    control_.desireSpeed(desireSpeed);
}

// ================= public functions =====================
XControl::XControl(const std::string& name) : XObject(name),
velocity("VELOCITY_ABS"),
rotation("ROTATION"),
desireSpeed("SPEED_CONTROL_ABS"),
flyControl("FLY_CONTROL")
{
    XBindVarF(velocity,&XControl::onChangeVelocity);
    XBindVarF(rotation, &XControl::onChangeRotation);
    XBindVarF(desireSpeed, &XControl::onChangeDesireSpeed);
    XBindVar(flyControl);
}

XStart(XControl);