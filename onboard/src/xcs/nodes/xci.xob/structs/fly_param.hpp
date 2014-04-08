#ifndef FLY_PARAM_HPP
#define	FLY_PARAM_HPP

#include <xcs/nodes/xobject/x.hpp>

namespace xcs {
namespace nodes {
namespace xci {

struct FlyParam {
    double roll;
    double pitch;
    double yaw;
    double gaz;

    FlyParam(double roll = 0, double pitch = 0, double yaw = 0, double gaz = 0) :
      roll(roll), pitch(pitch), yaw(yaw), gaz(gaz) {
    };
};
}
}
}

X_REGISTER_STRUCT(xcs::nodes::xci::FlyParam, roll, pitch, yaw, gaz);

#endif	/* FLY_PARAM_HPP */

