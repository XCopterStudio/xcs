#ifndef FLY_CONTROL_HPP
#define	FLY_CONTROL_HPP

#include <xcs/nodes/xobject/x.h>

namespace xcs {

struct FlyControl {
    double roll;
    double pitch;
    double yaw;
    double gaz;

    FlyControl(double roll = 0, double pitch = 0, double yaw = 0, double gaz = 0) :
      roll(roll), pitch(pitch), yaw(yaw), gaz(gaz) {
    };

    template<typename T>
            void serialize(T &stream) {
        stream << roll << "\t" << pitch << "\t" << yaw << "\t" << gaz;
    }
};
}

X_REGISTER_STRUCT(xcs::FlyControl, roll, pitch, yaw, gaz);

#endif	/* FLY_CONTROL_HPP */

