#ifndef SPEED_CONTROL_H
#define	SPEED_CONTROL_H

#include <xcs/nodes/xobject/x.h>

namespace xcs {

//TODO rename to velocity control

struct SpeedControl {
    double vx;
    double vy;
    double vz;
    double psi;

    SpeedControl(double vx = 0, double vy = 0, double vz = 0, double psi = 0) :
      vx(vx), vy(vy), vz(vz), psi(psi) {
    };

    template<typename T>
            void serialize(T &stream) {
        stream << vx << "\t" << vy << "\t" << vz << "\t" << psi;
    }

    template<typename T>
            static SpeedControl deserialize(T &stream) {
        double vx, vy, vz, psi;
        stream >> vx >> vy >> vz >> psi;
        return SpeedControl(vx, vy, vz);
    }
};
}

X_REGISTER_STRUCT(xcs::SpeedControl, vx, vy, vz, psi);

#endif