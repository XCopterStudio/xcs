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

EulerianVectorChronologic::EulerianVectorChronologic() :
    EulerianVector(),
    time(-1){
};

EulerianVectorChronologic::EulerianVectorChronologic(double phi, double theta, double psi, long int time) :
    EulerianVector(phi,theta, psi),
    time(time){
};

X_GENERATE_STRUCT(EulerianVectorChronologic, phi, psi, theta, time);