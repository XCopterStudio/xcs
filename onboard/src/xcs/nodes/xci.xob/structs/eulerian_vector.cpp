#include "eulerian_vector.hpp"

using namespace xcs::nodes::xci;

EulerianVector::EulerianVector() :
phi(0),
theta(0),
psi(0){
}

EulerianVector::EulerianVector(double phi, double theta, double psi) :
phi(phi),
theta(theta),
psi(psi){
}

X_GENERATE_STRUCT(EulerianVector, phi, theta, psi);