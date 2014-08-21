#include <xcs/settings.hpp>
#include <xcs/logging.hpp>

#include "x_control.hpp"

using namespace xcs;
using namespace xcs::nodes::control;

void XControl::onChangeVelocity(xcs::CartesianVector velocity){
    if (!stopped_){
        control_.velocity(velocity);
    }
}

void XControl::onChangeRotation(xcs::EulerianVector rotation){
    if (!stopped_){
        control_.rotation(rotation);
        flyControl = control_.computeControl();
    }
}

void XControl::onChangeDesireVelocity(xcs::SpeedControl desireVelocity){
    control_.desireVelocity(desireVelocity);
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

void XControl::stateChanged(XObject::State state) {
    switch(state) {
        case XObject::STATE_STARTED:
            stopped_ = false;
            break;
        case XObject::STATE_STOPPED:
            stopped_ = true;
            break;            
    }
}


// ================= public functions =====================
XControl::XControl(const std::string& name) : XObject(name),
velocity("VELOCITY_ABS"),
rotation("ROTATION"),
desireVelocity("SPEED_CONTROL_ABS"),
flyControl("FLY_CONTROL")
{
    stopped_ = true;

    XBindVarF(velocity,&XControl::onChangeVelocity);
    XBindVarF(rotation, &XControl::onChangeRotation);
    XBindVarF(desireVelocity, &XControl::onChangeDesireVelocity);
    
    XBindVar(flyControl);

    XBindFunction(XControl, loadParameters);
    XBindFunction(XControl, init);
}

void XControl::init(const std::string &file){
    loadParameters(file);
}

XStart(XControl);