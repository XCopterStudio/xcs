#ifndef EKF_H
#define EKF_H

#include <deque>
#include <utility>
#include <random>
#include <chrono>

#include <xcs/types/fly_control.hpp>
#include <xcs/types/cartesian_vector.hpp>
#include <xcs/types/eulerian_vector.hpp>

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
    // TODO operators
    arma::mat getMat() const;
    void Mat(const arma::mat &mat);

    inline TooN::Vector<10> getVector() const {
        return TooN::makeVector(position.x, position.y, position.z, velocity.x, velocity.y, velocity.z, angles.phi, angles.theta, angles.psi, angularRotationPsi);
    }

    inline void Vector(const TooN::Vector<10> &vector) {
        position.x = vector[0];
        position.y = vector[1];
        position.z = vector[2];
        velocity.x = vector[3];
        velocity.y = vector[4];
        velocity.z = vector[5];
        angles.phi = vector[6];
        angles.theta = vector[7];
        angles.psi = vector[8];
        angularRotationPsi = vector[9];
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
    // TODO operators
    arma::mat getMat() const;
};

struct CameraMeasurement {
    xcs::CartesianVector position;
    xcs::EulerianVector angles;
    // TODO operators
    arma::mat getMat() const;

    inline void Vector(const TooN::Vector<6> &vector) {
        position.x = vector[0];
        position.y = vector[1];
        position.z = vector[2];
        angles.phi = vector[3];
        angles.theta = vector[4];
        angles.psi = vector[5];
    }
};

typedef std::pair<DroneState, arma::mat> DroneStateDistribution;
typedef std::pair<DroneStateMeasurement, double> ImuMeasurementChronologic;
typedef std::pair<DroneStateDistribution, double> DroneStateDistributionChronologic;
typedef std::pair<xcs::FlyControl, double> FlyControlChronologic;

// Measurement consist from measurement of the drone state and timestamp. Timestamp is the time in milliseconds when measurement was created.
typedef std::deque< ImuMeasurementChronologic > ImuMeasurements;
// Drone state consist from the drone state and timestamp. Timestamp is the time in milliseconds when drone state was created.
typedef std::deque< DroneStateDistributionChronologic > DroneStates;
// FlyControl consist from send fly controls and timestamp. Timestamp is the time in milliseconds when drone received fly controls.
typedef std::deque< FlyControlChronologic > FlyControls;

typedef std::mt19937 Generator;
typedef std::normal_distribution<double> NormalDistribution;

class Ekf {
    DroneStates droneStates_;
    ImuMeasurements imuMeasurements_;
    FlyControls flyControls_;

    double parameters_[10];
    unsigned int IDCounter_;

    Generator randomGenerator_;
    NormalDistribution normalDistribution_;

    template <typename Deque>
    void clearUpToTime(Deque &deque, const double &time);

    template <typename Deque>
    int findNearest(Deque &deque, const double &time);
    int findMeasurementIndex(const int &ID);

    DroneStateDistributionChronologic predict(const DroneStateDistributionChronologic& state, const double &endTime); // predict from state up to end time
    DroneStateDistributionChronologic predictAndUpdateFromImu(const DroneStateDistributionChronologic& state, const double &endTime, bool saveInterResults = false);

    DroneStateDistribution predict(const DroneStateDistribution &state, const xcs::FlyControl &flyControl, const double &delta); // predict only one step
    DroneStateDistribution updateIMU(const DroneStateDistribution &state, const DroneStateMeasurement &imuMeasurement);
    DroneStateDistribution updateCam(const DroneStateDistribution &state, const CameraMeasurement &camMeasurement);
public:
    Ekf();

    /*!
     * \param timestamp EKF time.
     */
    void clearUpToTime(const double timestamp);

    /*!
     * \param timestamp EKF time.
     */
    void flyControl(const xcs::FlyControl &flyControl, const double &timestamp);

    /*!
     * \param timestamp EKF time.
     */
    void measurementImu(const DroneStateMeasurement &measurement, const double &timestamp);

    /*!
     * \param timestamp EKF time.
     */
    void measurementCam(const CameraMeasurement &measurement, const double &timestamp);

    /*!
     * \param time EKF time.
     */
    DroneState computeState(const double &time); // compute prediction state up to this time 
};

}
}
}

#endif
