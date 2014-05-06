#ifndef EULERIAN_VECTOR_HPP
#define	EULERIAN_VECTOR_HPP

#include <xcs/nodes/xobject/x.h>

namespace xcs {

struct EulerianVector {
    double phi; // x-axis
    double theta; // y-axis
    double psi; // z-axis

    EulerianVector(double phi = 0, double theta = 0, double psi = 0) : phi(phi), theta(theta), psi(psi) {
    }

    template<typename T>
            void serialize(T &stream) {
        stream << phi << "\t" << theta << "\t" << psi;
    }
};

struct EulerianVectorChronologic : public EulerianVector {
    long int time; // in milliseconds

    EulerianVectorChronologic(double phi = 0, double theta = 0, double psi = 0, long int time = -1) :
      EulerianVector(phi, theta, psi), time(time) {
    }

    EulerianVectorChronologic(const EulerianVector &vector, long int time = -1) :
      EulerianVector(vector), time(time) {
    }

    template<typename T>
            void serialize(T &stream) {
        stream << phi << "\t" << theta << "\t" << psi << "\t" << time;
    }
};

}

X_REGISTER_STRUCT(xcs::EulerianVector, phi, theta, psi);
X_REGISTER_STRUCT(xcs::EulerianVectorChronologic, phi, theta, psi, time);

#endif	/* EULERIAN_VECTOR_HPP */