#include "x_flycontrol_aggregator.hpp"
#include <xcs/xcs_fce.hpp>

using namespace xcs::nodes::aggregator;

void XFlyControlAggregator::onChangeRoll(const double roll){
    flyControl_.roll = valueInRange(roll,1.0);
    flyControl = flyControl_;
}

void XFlyControlAggregator::onChangePitch(const double pitch){
    flyControl_.pitch = valueInRange(pitch,1.0);
    flyControl = flyControl_;
}

void XFlyControlAggregator::onChangeYaw(const double yaw){
    flyControl_.yaw = valueInRange(yaw, 1.0);
    flyControl = flyControl_;
}

void XFlyControlAggregator::onChangeGaz(const double gaz){
    flyControl_.gaz = valueInRange(gaz, 1.0);
    flyControl = flyControl_;
}

//==================== public function ===================

XFlyControlAggregator::XFlyControlAggregator(const std::string& name) : 
    XObject(name), roll("ROLL"), pitch("PITCH"), yaw("YAW"), gaz("GAZ"),
    flyControl("FLY_CONTROL"){

    XBindVarF(roll,&XFlyControlAggregator::onChangeRoll);
    XBindVarF(pitch, &XFlyControlAggregator::onChangePitch);
    XBindVarF(yaw, &XFlyControlAggregator::onChangeYaw);
    XBindVarF(gaz, &XFlyControlAggregator::onChangeGaz);

    XBindVar(flyControl);
}

XStart(XFlyControlAggregator);