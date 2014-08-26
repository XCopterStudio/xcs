#include "x_pid.hpp"
#include <xcs/nodes/xobject/x.h>

using namespace xcs::nodes::pid;

void XPID::onChangeP(const PidType p){
    pid_.P(p);
}

void XPID::onChangeI(const PidType i){
    pid_.I(i);
}

void XPID::onChangeD(const PidType d){
    pid_.D(d);
}

void XPID::onChangeActualValue(const double actualValue){
    control = pid_.getCorrection(actualValue,desireValue_);
}

void XPID::onChangeDesireValue(const double desireValue){
    desireValue_ = desireValue;
}

//==== public function =======

XPID::XPID(const std::string& name) : XObject(name),
    P("PID_PARAM"), I("PID_PARAM"), D("PID_PARAM"), 
    actualValue("PID_VALUE"), desireValue("PID_VALUE"), control("PID_CONTROL"){
    
    desireValue_ = 0;

    XBindVarF(P, &XPID::onChangeP);
    XBindVarF(I, &XPID::onChangeI);
    XBindVarF(D, &XPID::onChangeD);
    XBindVarF(actualValue, &XPID::onChangeActualValue);
    XBindVarF(desireValue, &XPID::onChangeDesireValue);

    XBindVar(control);

}

XStart(XPID);