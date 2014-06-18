#ifndef EKF_H
#define EKF_H

#include <deque>
#include <utility>
#include <vector>
#include <random>
#include <chrono>
#include <boost/thread.hpp>

#include <xcs/types/fly_control.hpp>
#include <xcs/types/cartesian_vector.hpp>
#include <xcs/types/eulerian_vector.hpp>


#include "ekf_structs.hpp"

namespace xcs {
namespace nodes {
namespace localization {

typedef std::vector<double> Parameters;
typedef std::vector<double> Variances;
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
    /*! Used to control access to all of droneStates_, imuMeasurements_ and flyControls_. */
    boost::shared_mutex bigSharedMtx_;

    DroneStates droneStates_;
    ImuMeasurements imuMeasurements_;
    FlyControls flyControls_;

    double modelPar_[10];
    double modelVariance_[4];
    double imuVariance_[7];
    double ptamVariance_[6];
    unsigned int IDCounter_;

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
    Ekf(Parameters parameters, Variances model, Variances imu, Variances ptam);

    void modelParameters(Parameters parameters);
    void modelVariances(Variances model);
    void imuVariances(Variances imu);
    void ptamVariances(Variances ptam);
    /*!
     * \param timestamp EKF time.
     */
    void clearUpToTime(const double timestamp);

    /*!
     * \param timestamp EKF time.
     */
    void flyControl(const xcs::FlyControl flyControl, const double timestamp);

    /*!
     * \param timestamp EKF time.
     */
    void measurementImu(const DroneStateMeasurement &measurement, const double timestamp);

    /*!
     * \param timestamp EKF time.
     */
    void measurementCam(const CameraMeasurement &measurement, const double timestamp);

    /*!
     * \param time EKF time.
     */
    DroneState computeState(const double time); // compute prediction state up to this time 
};

}
}
}

#endif
