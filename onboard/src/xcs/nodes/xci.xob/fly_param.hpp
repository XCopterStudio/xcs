#ifndef FLY_PARAM_HPP
#define	FLY_PARAM_HPP

#include <xcs/nodes/xobject/x.hpp>

struct FlyParam {
    double roll;
    double pitch;
    double yaw;
    double gaz;
    FlyParam();
    FlyParam(double roll, double pitch, double yaw, double gaz);
};

X_REGISTER_STRUCT(FlyParam, roll, pitch, yaw, gaz);

#endif	/* FLY_PARAM_HPP */

