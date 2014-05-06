#ifndef CARTESIAN_VECTOR_HPP
#define	CARTESIAN_VECTOR_HPP

#include <xcs/nodes/xobject/x.h>

namespace xcs {

struct CartesianVector {
    double x;
    double y;
    double z;

    CartesianVector(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) {
    }

    template<typename T>
            void serialize(T &stream) {
        stream << x << "\t" << y << "\t" << z;
    }    
};

struct CartesianVectorChronologic : public CartesianVector {
    long int time; // in milliseconds

    CartesianVectorChronologic(double x = 0, double y = 0, double z = 0, long int time = -1) :
      CartesianVector(x, y, z), time(time) {
    }
    
    CartesianVectorChronologic(const CartesianVector &vector, long int time = -1) :
      CartesianVector(vector), time(time) {
    }

    template<typename T>
            void serialize(T &stream) {
        stream << x << "\t" << y << "\t" << z << "\t" << time;
    }
};

}

X_REGISTER_STRUCT(xcs::CartesianVector, x, y, z);
X_REGISTER_STRUCT(xcs::CartesianVectorChronologic, x, y, z, time);

#endif	/* CARTESIAN_VECTOR_HPP */