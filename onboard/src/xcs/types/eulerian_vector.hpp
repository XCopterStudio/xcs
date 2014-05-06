#ifndef EULERIAN_VECTOR_HPP
#define	EULERIAN_VECTOR_HPP

#include <xcs/nodes/xobject/x.h>
#include "timestamp.hpp"

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

    template<typename T>
            static EulerianVector deserialize(T &stream) {
        double phi, theta, psi;
        stream >> phi >> theta >> psi;
        return EulerianVector(phi, theta, psi);
    }
};

struct EulerianVectorChronologic : public EulerianVector {
    Timestamp time; // in milliseconds

    EulerianVectorChronologic(double phi = 0, double theta = 0, double psi = 0, Timestamp time = -1) :
      EulerianVector(phi, theta, psi), time(time) {
    }

    EulerianVectorChronologic(const EulerianVector &vector, Timestamp time = -1) :
      EulerianVector(vector), time(time) {
    }

    template<typename T>
            void serialize(T &stream) {
        stream << phi << "\t" << theta << "\t" << psi << "\t" << time;
    }

    template<typename T>
            static EulerianVectorChronologic deserialize(T &stream) {
        double phi, theta, psi;
        Timestamp time;
        stream >> phi >> theta >> psi >> time;
        return EulerianVectorChronologic(phi, theta, psi, time);
    }
};

}

X_REGISTER_STRUCT(xcs::EulerianVector, phi, theta, psi);
X_REGISTER_STRUCT(xcs::EulerianVectorChronologic, phi, theta, psi, time);

#endif	/* EULERIAN_VECTOR_HPP */