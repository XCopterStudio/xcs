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
    FlyParam();
    FlyParam(double roll, double pitch, double yaw, double gaz);
};
}
}
}

X_REGISTER_STRUCT(xcs::nodes::xci::FlyParam, roll, pitch, yaw, gaz);

#endif	/* FLY_PARAM_HPP */

