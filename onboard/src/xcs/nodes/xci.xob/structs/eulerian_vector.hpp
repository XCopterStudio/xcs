#ifndef EULERIAN_VECTOR_HPP
#define	EULERIAN_VECTOR_HPP

#include <xcs/nodes/xobject/x.hpp>

namespace xcs {
namespace nodes {
namespace xci {

    struct EulerianVector {
        double phi; // x-axis
        double psi; // y-axis
        double theta; // z-axis
        EulerianVector();
        EulerianVector(double phi, double psi, double theta);
    };

    struct EulerianVectorChronologic : public EulerianVector{
        long int time; // in milliseconds
        EulerianVectorChronologic();
        EulerianVectorChronologic(double phi, double psi, double theta, long int time);
    };
}}}

X_REGISTER_STRUCT(xcs::nodes::xci::EulerianVectorChronologic, phi, psi, theta, time);

#endif	/* EULERIAN_VECTOR_HPP */