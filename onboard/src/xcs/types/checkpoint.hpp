#ifndef CHECKPOINT_H
#define	CHECKPOINT_H

#include <xcs/nodes/xobject/x.hpp>

namespace xcs {

    struct Checkpoint {
        double x;
        double y;
        double z;

        double xOut;
        double yOut;
        double zOut;

        Checkpoint(double x = 0, double y = 0, double z = 0, double xOut = 0, double yOut = 0, double zOut = 0) : 
            x(x), y(y), z(z),
            xOut(xOut), yOut(yOut), zOut(zOut) {
        }

        template<typename T>
        void serialize(T &stream) {
            stream << x << "\t" << y << "\t" << z << "\t" << xOut << "\t" << yOut << "\t" << zOut;
        }
    };

}

X_REGISTER_STRUCT(xcs::Checkpoint, x, y, z, xOut, yOut, zOut);

#endif	