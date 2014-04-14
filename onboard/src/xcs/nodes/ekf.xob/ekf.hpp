#ifndef EKF_H
#define EKF_H

#include <queue>
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

        arma::mat getMat() const;
        void Mat(const arma::mat &mat);
    };

    struct DroneStateMeasurement{
        xcs::CartesianVector velocity;
        xcs::EulerianVector angles;
        double altitude;
        double angularRotationPsi;

        arma::mat getMat() const;
    };

    typedef std::pair<DroneState, arma::mat> DroneStateDistribution;
    typedef std::pair<DroneStateMeasurement, long int> MeasurementChronologic;
    typedef std::pair<DroneStateDistribution, long int> DroneStateDistributionChronologic;
    typedef std::pair<xcs::FlyControl, long int> FlyControlChronologic;

    // Measurement consist from measurement of the drone state and timestamp. Timestamp is the time in milliseconds when measurement was created.
    typedef std::queue< MeasurementChronologic > Measurements;
    // Drone state consist from the drone state and timestamp. Timestamp is the time in milliseconds when drone state was created.
    typedef std::queue< DroneStateDistributionChronologic > DroneStates;
    // FlyControl consist from send fly controls and timestamp. Timestamp is the time in milliseconds when drone received fly controls.
    typedef std::queue< FlyControlChronologic > FlyControls;

    typedef std::mt19937 Generator;
    typedef std::normal_distribution<double> NormalDistribution;

    typedef std::chrono::high_resolution_clock Clock;

    class Ekf{
        DroneStates droneStates_;
        DroneStates droneStateDeviation_;
        Measurements measurements_;
        FlyControls flyControls_;

        double parameters_[10];

        Generator randomGenerator_;
        NormalDistribution normalDistribution_;

        std::chrono::high_resolution_clock::time_point startTime_;

        DroneStateDistribution predict(const DroneStateDistribution &state, const xcs::FlyControl &flyControl, const double &delta);
        DroneStateDistribution updateIMU(const DroneStateDistribution &state, const DroneStateMeasurement &imuMeasurement);
    public:
        Ekf();
        void flyControl(const xcs::FlyControl &flyControl, const long int &timestamp);
        void measurement(const DroneStateMeasurement &measurement, const long int &timestamp);
        DroneStateDistribution predict();
        
    };

}}}

#endif
