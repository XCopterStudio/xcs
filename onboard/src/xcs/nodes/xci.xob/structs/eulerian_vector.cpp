#include "eulerian_vector.hpp"

using namespace xcs::nodes::xci;

EulerianVector::EulerianVector() :
    phi(0),
    theta(0),
    psi(0){
}

EulerianVector::EulerianVector(double phi, double psi, double theta) :
    phi(phi),
    psi(psi),
    theta(theta){
}

EulerianVectorChronologic::EulerianVectorChronologic() :
    EulerianVector(),
    time(-1){
};

EulerianVectorChronologic::EulerianVectorChronologic(double phi, double psi, double theta, long int time) :
    EulerianVector(phi,psi,theta),
    time(time){
};

X_GENERATE_STRUCT(EulerianVectorChronologic, phi, psi, theta, time);