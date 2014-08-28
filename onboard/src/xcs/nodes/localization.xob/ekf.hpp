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

//! Measurement consist from measurement of the drone state and timestamp. Timestamp is the time in milliseconds when measurement was created.
typedef std::deque< ImuMeasurementChronologic > ImuMeasurements;
//! Drone state consist from the drone state and timestamp. Timestamp is the time in milliseconds when drone state was created.
typedef std::deque< DroneStateDistributionChronologic > DroneStates;
//! FlyControl consist from send fly controls and timestamp. Timestamp is the time in milliseconds when drone received fly controls.
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
    double imuVariance_[6];
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
    //! Initialize ekf with zeros and default parameters
    Ekf();
    /*! Initialize ekf with custom parameters

        \param parameters of the quadricopter movement model
        \param model variance of the quadricopter movement model
        \param imu variance of the imu measurements from quadricopter
        \param ptam variance of the computed quadricopter state from ptam framework
    */
    Ekf(Parameters parameters, Variances model, Variances imu, Variances ptam);

    /*! Set quadricopter movement model parameters.
        
        \param parameters of the quadricopter movement model
    */
    void modelParameters(Parameters parameters);
    /*! Set variance for quadricopter movement model.

        \param model variance of the quadricopter movement model
    */
    void modelVariances(Variances model);
    /*! Set variance for imu measurement from an quadricopter.

        \param imu variance of the imu measurements from quadricopter
    */
    void imuVariances(Variances imu);
    /*! Set variance for computed quadricopter state from ptam framework.

        \param ptam variance of the computed quadricopter state from ptam framework
    */
    void ptamVariances(Variances ptam);
    /*! Clear items from droneStates_, imuMeasurements_ and flyControls_ up to this time and keep only first item with time stamp lower than input timestamp.

        \param timestamp EKF time.
     */
    void clearUpToTime(const double timestamp);

    /*! Insert new FlyControl in Ekf with timestamp.

        \param flyControl quadricopter fly control
        \param timestamp EKF time when flyControl take it is effect on an quadricopter
     */
    void flyControl(const xcs::FlyControl flyControl, const double timestamp);

    /*! Insert new DroneStateMeasurement in Ekf with timestamp.

        \param measurement from an quadricopter imu sensors
        \param timestamp EKF time of the capturing imu measurement.
     */
    void measurementImu(const DroneStateMeasurement &measurement, const double timestamp);

    /*! Insert new CameraMeasurement in Ekf with timestamp.

        \param measurement from ptam framework
        \param timestamp EKF time when quadricopter was on this computed measurement
     */
    void measurementCam(const CameraMeasurement &measurement, const double timestamp);

    /*! State of an quadricopter in desire Ekf time.
        Look only on small time interval around actual measurements if you periodicaly call clearUpToTime().

        \param time EKF time.
     */
    DroneState computeState(const double time); // compute prediction state up to this time 

    /*! Set ekf position and delete all old items up to this time.
    
        \param position new ekf position
        \param timestamp ekf time of the position
    */
    void setPosition(const xcs::CartesianVector position, const double timestamp);
    /*! Set ekf rotation and delete all old items up to this time..

    \param rotation new ekf rotation
    \param timestamp ekf time of the rotation
    */
    void setRotation(const xcs::EulerianVector rotation, const double timestamp);

    /*! Reset whole ekf to the default parameters. */
    void reset();
};

}
}
}

#endif
