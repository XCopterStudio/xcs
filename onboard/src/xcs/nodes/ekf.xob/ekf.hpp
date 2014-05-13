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

namespace xcs{
namespace nodes{
namespace ekf{

    struct DroneState{
        xcs::CartesianVector position;
        xcs::CartesianVector velocity;
        xcs::EulerianVector angles;
        double angularRotationPsi;

        DroneState() : angularRotationPsi(0) {  };
        arma::mat getMat() const;
        void Mat(const arma::mat &mat);
    };

    struct DroneStateMeasurement{
        xcs::CartesianVector velocity;
        xcs::EulerianVector angles;
        double altitude;
        double angularRotationPsi;

        DroneStateMeasurement() : altitude(0), angularRotationPsi(0){};
        arma::mat getMat() const;
    };

    typedef std::pair<DroneState, arma::mat> DroneStateDistribution;
    typedef std::pair<DroneStateMeasurement, double> MeasurementChronologic;
    typedef std::pair<DroneStateDistribution, double> DroneStateDistributionChronologic;
    typedef std::pair<xcs::FlyControl, double> FlyControlChronologic;

    // Measurement consist from measurement of the drone state and timestamp. Timestamp is the time in milliseconds when measurement was created.
    typedef std::deque< MeasurementChronologic > Measurements;
    // Drone state consist from the drone state and timestamp. Timestamp is the time in milliseconds when drone state was created.
    typedef std::deque< DroneStateDistributionChronologic > DroneStates;
    // FlyControl consist from send fly controls and timestamp. Timestamp is the time in milliseconds when drone received fly controls.
    typedef std::deque< FlyControlChronologic > FlyControls;

    typedef std::mt19937 Generator;
    typedef std::normal_distribution<double> NormalDistribution;

    class Ekf{
        DroneStates droneStates_;
        Measurements measurements_;
        FlyControls flyControls_;

        double lastStateTime; // Time of state which was fully computed from all available sensors.
        double parameters_[10];

        Generator randomGenerator_;
        NormalDistribution normalDistribution_;

        template <typename Deque>
        int findNearest(Deque &deque, const double &time);
        template <typename Deque>
        Deque findAllBetween(Deque &deque, const double &beginTime, const double &endTime);
        DroneStateDistributionChronologic predictAndUpdateFromImu(const double &beginTime, const double &endTime);

        DroneStateDistribution predict(const DroneStateDistribution &state, const xcs::FlyControl &flyControl, const double &delta);
        DroneStateDistribution updateIMU(const DroneStateDistribution &state, const DroneStateMeasurement &imuMeasurement);
    public:
        Ekf();
        void flyControl(const xcs::FlyControl &flyControl, const double &timestamp);
        void measurement(const DroneStateMeasurement &measurement, const double &timestamp);  
        DroneState computeState(const double &time); // compute prediction state up to this time 
    };

}}}

#endif
