#include "eulerian_vector.hpp"
#include <xcs/nodes/xobject/x_impl.hpp>

using namespace xcs;

X_GENERATE_STRUCT(EulerianVector, phi, psi, theta);
X_GENERATE_STRUCT(EulerianVectorChronologic, phi, psi, theta, time);