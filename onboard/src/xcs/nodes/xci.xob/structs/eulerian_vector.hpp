#ifndef EULERIAN_VECTOR_HPP
#define	EULERIAN_VECTOR_HPP

#include <xcs/nodes/xobject/x.hpp>

namespace xcs {
namespace nodes {
namespace xci {

    struct EulerianVector {
        double phi; // x-axis
        double theta; // y-axis
        double psi; // z-axis
        EulerianVector();
        EulerianVector(double phi, double theta, double psi);
    };

    struct EulerianVectorChronologic : public EulerianVector{
        long int time; // in milliseconds
        EulerianVectorChronologic();
        EulerianVectorChronologic(double phi, double theta, double psi, long int time);
    };
}}}

X_REGISTER_STRUCT(xcs::nodes::xci::EulerianVectorChronologic, phi, theta, psi, time);

#endif	/* EULERIAN_VECTOR_HPP */