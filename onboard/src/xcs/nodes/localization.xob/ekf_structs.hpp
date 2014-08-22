#ifndef EKF_STRUCTS_HPP
#define	EKF_STRUCTS_HPP

#include "tum/tum_utils.hpp"
#include <armadillo>
#include <TooN/TooN.h>

namespace xcs {
namespace nodes {
namespace localization {

//! Preserve all information about quadricopters state.
struct DroneState {
    xcs::CartesianVector position;
    xcs::CartesianVector velocity;
    xcs::EulerianVector rotation;
    double velocityPsi;

    unsigned int updateMeasurementID;

    /*! Fill struct parameters

        \param position quadricopter position in 3 dimensional space in meters
        \param velocity quadricopter velocity in 3 dimensional space in m/s
        \param angles   quadricopter rotation around 3 axis
        \param velocityPsi quadricopter rotation velocity around z axis 
        \param updateMeasurementID correspond with Imu data ID in Ekf. Do not change it!!!
    */
    inline DroneState(xcs::CartesianVector position = xcs::CartesianVector(), xcs::CartesianVector velocity = xcs::CartesianVector(),
        xcs::EulerianVector angles = xcs::EulerianVector(), double velocityPsi = 0,
        unsigned int updateMeasurementID = 0) : position(position), velocity(velocity),
        rotation(angles), velocityPsi(velocityPsi), 
        updateMeasurementID(updateMeasurementID) {
    };

    //! Convert quadricopter state represented as vector to the DroneState struction
    inline DroneState(const arma::mat &mat) :
      position(mat.at(0, 0), mat.at(1, 0), mat.at(2, 0)),
      velocity(mat.at(3, 0), mat.at(4, 0), mat.at(5, 0)),
      rotation(mat.at(6, 0), mat.at(7, 0), mat.at(8, 0)),
      velocityPsi(mat.at(9, 0)),
      updateMeasurementID(0) {
    }

    //! Return DroneState struction as arma::mat quadricopter state vector 
    inline operator arma::mat() const {
        arma::mat matrix;
        matrix << position.x << arma::endr
                << position.y << arma::endr
                << position.z << arma::endr
                << velocity.x << arma::endr
                << velocity.y << arma::endr
                << velocity.z << arma::endr
                << rotation.phi << arma::endr
                << rotation.theta << arma::endr
                << rotation.psi << arma::endr
                << velocityPsi << arma::endr;
        return matrix;
    }

    //! Return DroneState struction as TooN::Vector quadricopter state vector 
    inline operator TooN::Vector<10> () const {
        return TooN::makeVector(position.x, position.y, position.z, rotation.phi, rotation.theta, rotation.psi, velocity.x, velocity.y, velocity.z, velocityPsi);
    }

};

//! Measurement of an quadricopter state
struct DroneStateMeasurement {
    xcs::CartesianVector velocity;
    xcs::EulerianVector rotation;
    double altitude;
    double velocityPsi;

    unsigned int measurementID;

    //! Fill variables with zeros
    DroneStateMeasurement() : altitude(0), velocityPsi(0), measurementID(0) {
    };

    //! Convert on arma::mat vector
    inline operator arma::mat() const {
        arma::mat measurement;
        measurement << velocity.x << arma::endr
                << velocity.y << arma::endr
                << velocity.z << arma::endr
                << rotation.phi << arma::endr
                << rotation.theta << arma::endr
                << velocityPsi << arma::endr;
        return measurement;
    }
};

//! Measurement of the quadricopter position and rotation from camera
struct CameraMeasurement {
    xcs::CartesianVector position;
    xcs::EulerianVector rotation;

    //! Convert on arma::mat vector
    inline operator arma::mat() const {
        arma::mat measurement;
        measurement << position.x << arma::endr
                << position.y << arma::endr
                << position.z << arma::endr
                << rotation.phi << arma::endr
                << rotation.theta << arma::endr
                << rotation.psi << arma::endr;
        return measurement;
    }

    //! Create CameraMeasurement from Toon::SE3 
    inline CameraMeasurement(const TooN::SE3<> &pose) {
        position.x = pose.get_translation()[0];
        position.y = pose.get_translation()[1];
        position.z = pose.get_translation()[2];
        const auto poseAngles = so3ToAngles(pose.get_rotation());
        rotation.phi = poseAngles[0];
        rotation.theta = poseAngles[1];
        rotation.psi = poseAngles[2];
    }
};
}
}
}


#endif	/* EKF_STRUCTS_HPP */

