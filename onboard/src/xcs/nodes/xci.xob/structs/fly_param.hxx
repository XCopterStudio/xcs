#include "fly_param.hpp"
#include <xcs/nodes/xobject/x.hpp>

xcs::nodes::xci::FlyParam::FlyParam() :
    roll(0),
    pitch(0),
    yaw(0),
    gaz(0) {    
}

xcs::nodes::xci::FlyParam::FlyParam(double r, double p, double y, double g) :
    roll(r),
    pitch(p),
    yaw(y),
    gaz(g) {
}

X_GENERATE_STRUCT(xcs::nodes::xci::FlyParam, roll, pitch, yaw, gaz);