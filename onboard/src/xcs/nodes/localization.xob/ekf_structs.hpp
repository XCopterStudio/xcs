#ifndef EKF_STRUCTS_HPP
#define	EKF_STRUCTS_HPP

#include <armadillo>
#include <TooN/TooN.h>

namespace xcs {
namespace nodes {
namespace localization {

struct DroneState {
    xcs::CartesianVector position;
    xcs::CartesianVector velocity;
    xcs::EulerianVector angles;
    double angularRotationPsi;

    unsigned int updateMeasurementID;

    DroneState() : angularRotationPsi(0), updateMeasurementID(0) {
    };

    inline DroneState(const TooN::Vector<10> &vector) :
      position(vector[0], vector[1], vector[2]),
      velocity(vector[3], vector[4], vector[5]),
      angles(vector[6], vector[7], vector[8]),
      angularRotationPsi(vector[9]),
      updateMeasurementID(0) {
    }

    inline DroneState(const arma::mat &mat) :
      position(mat.at(0, 0), mat.at(1, 0), mat.at(2, 0)),
      velocity(mat.at(3, 0), mat.at(4, 0), mat.at(5, 0)),
      angles(mat.at(6, 0), mat.at(7, 0), mat.at(8, 0)),
      angularRotationPsi(mat.at(9, 0)),
      updateMeasurementID(0) {
    }

    inline operator arma::mat() const {
        arma::mat matrix;
        matrix << position.x << arma::endr
                << position.y << arma::endr
                << position.z << arma::endr
                << velocity.x << arma::endr
                << velocity.y << arma::endr
                << velocity.z << arma::endr
                << angles.phi << arma::endr
                << angles.theta << arma::endr
                << angles.psi << arma::endr
                << angularRotationPsi << arma::endr;
        return matrix;
    }

    inline operator TooN::Vector<10> () const {
        return TooN::makeVector(position.x, position.y, position.z, angles.phi, angles.theta, angles.psi, velocity.x, velocity.y, velocity.z, angularRotationPsi);
    }

};

struct DroneStateMeasurement {
    xcs::CartesianVector velocity;
    xcs::EulerianVector angles;
    double altitude;
    double angularRotationPsi;

    unsigned int measurementID;

    DroneStateMeasurement() : altitude(0), angularRotationPsi(0), measurementID(0) {
    };

    inline operator arma::mat() const {
        arma::mat measurement;
        measurement << altitude << arma::endr
                << velocity.x << arma::endr
                << velocity.y << arma::endr
                << velocity.z << arma::endr
                << angles.phi << arma::endr
                << angles.theta << arma::endr
                << angularRotationPsi << arma::endr;
        return measurement;
    }
};

struct CameraMeasurement {
    xcs::CartesianVector position;
    xcs::EulerianVector angles;

    inline operator arma::mat() const {
        arma::mat measurement;
        measurement << position.x << arma::endr
                << position.y << arma::endr
                << position.z << arma::endr
                << angles.phi << arma::endr
                << angles.theta << arma::endr
                << angles.psi << arma::endr;
        return measurement;
    }

    inline CameraMeasurement(const TooN::Vector<6> &vector) :
      position(vector[0], vector[1], vector[2]),
      angles(vector[3], vector[4], vector[5]) {
    }
};
}
}
}


#endif	/* EKF_STRUCTS_HPP */

