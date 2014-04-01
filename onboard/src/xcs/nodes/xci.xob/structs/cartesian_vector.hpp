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
        CartesianVector();
        CartesianVector(double x, double y, double z);
    };

    struct CartesianVectorChronologic : public CartesianVector{
        long int time; // in milliseconds
        CartesianVectorChronologic();
        CartesianVectorChronologic(double x, double y, double z, long int time);
    };

}}}

X_REGISTER_STRUCT(xcs::nodes::xci::CartesianVectorChronologic, x, y, z, time);

#endif	/* CARTESIAN_VECTOR_HPP */