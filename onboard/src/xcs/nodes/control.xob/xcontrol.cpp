#include <xcs/settings.hpp>
#include <xcs/logging.hpp>

#include "xcontrol.hpp"

using namespace xcs;
using namespace xcs::nodes::control;

void XControl::onChangeVelocity(xcs::CartesianVector velocity){
    if (!stoped){
        control_.velocity(velocity);
    }
}

void XControl::onChangeRotation(xcs::EulerianVector rotation){
    if (!stoped){
        control_.rotation(rotation);
        flyControl = control_.computeControl();
    }
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
    stoped = true;

    XBindVarF(velocity,&XControl::onChangeVelocity);
    XBindVarF(rotation, &XControl::onChangeRotation);
    XBindVarF(desireSpeed, &XControl::onChangeDesireSpeed);
    
    XBindVar(flyControl);

    XBindFunction(XControl, loadParameters);
    XBindFunction(XControl, start);
    XBindFunction(XControl, stop);
}

void XControl::loadParameters(const std::string &file){
    Settings settings(file);

    try{
        control_.setVxPID(settings.get<PID_TYPE>("vxPID.P"), settings.get<PID_TYPE>("vxPID.I"), settings.get<PID_TYPE>("vxPID.D"));
        control_.setVyPID(settings.get<PID_TYPE>("vyPID.P"), settings.get<PID_TYPE>("vyPID.I"), settings.get<PID_TYPE>("vyPID.D"));
        control_.setVzPID(settings.get<PID_TYPE>("vzPID.P"), settings.get<PID_TYPE>("vzPID.I"), settings.get<PID_TYPE>("vzPID.D"));
        control_.setPsiPID(settings.get<PID_TYPE>("psiPID.P"), settings.get<PID_TYPE>("psiPID.I"), settings.get<PID_TYPE>("psiPID.D"));
    }
    catch (boost::property_tree::ptree_error error){
        XCS_LOG_ERROR("Control cannot load parameters from file " << file << "\n With boost error: " << error.what());
    }
}

XStart(XControl);