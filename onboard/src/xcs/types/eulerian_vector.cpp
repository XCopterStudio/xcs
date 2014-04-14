#include "eulerian_vector.hpp"

using namespace xcs;

X_GENERATE_STRUCT(EulerianVector, phi, psi, theta);
X_GENERATE_STRUCT(EulerianVectorChronologic, phi, psi, theta, time);