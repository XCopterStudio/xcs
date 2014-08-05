#include "eulerian_vector.hpp"
#include <xcs/nodes/xobject/x_impl.hpp>

#undef X_STRUCT_NAMESPACE
#define X_STRUCT_NAMESPACE "xcs::"
X_GENERATE_STRUCT(EulerianVector, phi, psi, theta);
X_GENERATE_STRUCT(EulerianVectorChronologic, phi, psi, theta, time);