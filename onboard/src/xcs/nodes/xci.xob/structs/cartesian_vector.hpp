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

}}}

X_REGISTER_STRUCT(xcs::nodes::xci::CartesianVector, x, y, z);

#endif	/* CARTESIAN_VECTOR_HPP */