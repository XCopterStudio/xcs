#ifndef SPEED_CONTROL_H
#define	SPEED_CONTROL_H

#include <xcs/nodes/xobject/x.h>

namespace xcs {

struct VelocityControl {
    double vx;
    double vy;
    double vz;
    double psi;

    VelocityControl(double vx = 0, double vy = 0, double vz = 0, double psi = 0) :
      vx(vx), vy(vy), vz(vz), psi(psi) {
    };

    template<typename T>
            void serialize(T &stream) {
        stream << vx << "\t" << vy << "\t" << vz << "\t" << psi;
    }

    template<typename T>
            static VelocityControl deserialize(T &stream) {
        double vx, vy, vz, psi;
        stream >> vx >> vy >> vz >> psi;
        return VelocityControl(vx, vy, vz);
    }
};
}

X_REGISTER_STRUCT(xcs::VelocityControl, vx, vy, vz, psi);

#endif