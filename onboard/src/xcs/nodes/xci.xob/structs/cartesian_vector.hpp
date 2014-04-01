#ifndef CARTESIAN_VECTOR_HPP
#define	CARTESIAN_VECTOR_HPP

#include <xcs/nodes/xobject/x.hpp>

namespace xcs {
namespace nodes {
namespace xci {

struct CartesianVector {
    double x;
    double y;
    double z;

    CartesianVector(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) {
    }
};

struct CartesianVectorChronologic : public CartesianVector {
    long int time; // in milliseconds

    CartesianVectorChronologic(double x = 0, double y = 0, double z = 0, long int time = -1) :
      CartesianVector(x, y, z), time(time) {
    }
};

}
}
}

X_REGISTER_STRUCT(xcs::nodes::xci::CartesianVectorChronologic, x, y, z, time);

#endif	/* CARTESIAN_VECTOR_HPP */