#ifndef CARTESIAN_VECTOR_HPP
#define	CARTESIAN_VECTOR_HPP

#include <xcs/nodes/xobject/x.h>
#include "timestamp.hpp"

namespace xcs {

struct CartesianVector {
    double x;
    double y;
    double z;

    CartesianVector(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) {
    }

    inline CartesianVector operator*(const double scale) const {
        return CartesianVector(scale*x, scale*y, scale * z);
    }

    inline CartesianVector &operator+=(const CartesianVector &rhs) {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }

    template<typename T>
            void serialize(T &stream) {
        stream << x << "\t" << y << "\t" << z;
    }

    template<typename T>
            static CartesianVector deserialize(T &stream) {
        double x, y, z;
        stream >> x >> y >> z;
        return CartesianVector(x, y, z);
    }
};

struct CartesianVectorChronologic : public CartesianVector {
    Timestamp time; // in milliseconds

    CartesianVectorChronologic(double x = 0, double y = 0, double z = 0, Timestamp time = -1) :
      CartesianVector(x, y, z), time(time) {
    }

    CartesianVectorChronologic(const CartesianVector &vector, Timestamp time = -1) :
      CartesianVector(vector), time(time) {
    }

    template<typename T>
            void serialize(T &stream) {
        stream << x << "\t" << y << "\t" << z << "\t" << time;
    }

    template<typename T>
            static CartesianVectorChronologic deserialize(T &stream) {
        double x, y, z;
        Timestamp time;
        stream >> x >> y >> z >> time;
        return CartesianVectorChronologic(x, y, z, time);
    }
};

}

X_REGISTER_STRUCT(xcs::CartesianVector, x, y, z);
X_REGISTER_STRUCT(xcs::CartesianVectorChronologic, x, y, z, time);

#endif	/* CARTESIAN_VECTOR_HPP */