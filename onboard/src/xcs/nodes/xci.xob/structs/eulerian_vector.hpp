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

        }
    }
}

X_REGISTER_STRUCT(xcs::nodes::xci::EulerianVector, phi, theta, psi);

#endif	/* EULERIAN_VECTOR_HPP */