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

        unsigned int updateMeasurementID;

        DroneState() : angularRotationPsi(0), updateMeasurementID(0) {  };
        arma::mat getMat() const;
        void Mat(const arma::mat &mat);
    };

    struct DroneStateMeasurement{
        xcs::CartesianVector velocity;
        xcs::EulerianVector angles;
        double altitude;
        double angularRotationPsi;

        unsigned int measurementID;

        DroneStateMeasurement() : altitude(0), angularRotationPsi(0), measurementID(0){};
        arma::mat getMat() const;
    };

    struct CameraMeasurement{
        xcs::CartesianVector position;
        xcs::EulerianVector angles;

        arma::mat getMat() const;
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

    class Ekf{
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
        void clearUpToTime(const double timestamp);
        void flyControl(const xcs::FlyControl &flyControl, const double &timestamp);
        void measurementImu(const DroneStateMeasurement &measurement, const double &timestamp);  
        void measurementCam(const CameraMeasurement &measurement, const double &timestamp);
        DroneState computeState(const double &time); // compute prediction state up to this time 
    };

}}}

#endif
