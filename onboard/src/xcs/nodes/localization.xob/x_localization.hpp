#ifndef X_LOCALIZATION_H
#define X_LOCALIZATION_H

#include <chrono>
#include <memory>
#include <mutex>
#include <atomic>

#include "ekf.hpp"
#include "ptam.hpp"
#include <xcs/xcs_fce.hpp>

#include <xcs/nodes/xobject/x_object.hpp>
#include <xcs/nodes/xobject/x_input_port.hpp>
#include <xcs/nodes/xobject/x_var.hpp>

#include <xcs/types/cartesian_vector.hpp>
#include <xcs/types/eulerian_vector.hpp>

namespace xcs {
namespace nodes {

typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::time_point<std::chrono::high_resolution_clock> TimePoint;

/*! \brief XObject wrapper for quadricopter localization in 3 dimensional space.

    \warning Update data in input ports in following order for 
        imu measurements: measuredVelocity, measuredRotation, measuredAltitude, timeImu
        cam measurements: video, videoTime
        flyControls: flyControl, flyControlSendTime
        because only in time update is created appropriate structure and pass in Localization
        other methods only set temporally copy of the input data
*/
class XLocalization : public XObject {
    typedef std::unique_ptr<localization::Ptam> PtamPtr;

    static const double IMU_DELAY;
    static const double FLY_CONTROL_SEND_TIME;
    static const double CAM_DELAY;
    
    static const std::string CTRL_INIT_KF;
    static const std::string CTRL_TAKE_KF;
    static const std::string CTRL_RESET_PTAM;
    static const std::string CTRL_RESET_EKF;

    localization::Ekf ekf_;

    PtamPtr ptam_;

    localization::DroneStateMeasurement lastMeasurement_;
    double lastMeasurementTime_;


    Clock clock_;
    TimePoint startTime_;
    double imuTimeShift_;
    double flyControlSendTime_;
    ::urbi::UImage lastFrame_;
    std::mutex lastFrameMtx_;
    std::atomic<bool> ptamEnabled_;

    void onChangeVelocity(xcs::CartesianVector measuredVelocity);
    void onChangeRotation(xcs::EulerianVector measuredRotation);
    void onChangeAltitude(double altitude);
    // change time after you change all imu measurements (velocity etc.) 
    // because this add new imu measurement in ekf queue
    void onChangeTimeImu(double internalTime);

    void onChangeVideo(::urbi::UImage image);
    void onChangeVideoTime(xcs::Timestamp internalTime);

    void onChangeFlyControl(const xcs::FlyControl flyControl);
    void onChangeFlyControlSendTime(const double flyControlSendTime);

    void onChangeControl(const std::string &control);
    
    void onChangePtamEnabled(const bool ptamEnabled);

    void onChangeSetPosition(xcs::CartesianVector position);
    void onChangeSetRotation(xcs::EulerianVector rotation);

    inline double timeFromStart() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
                clock_.now() - startTime_).count() / 1000.0;
    };

    void loadParameters(const xcs::Settings &settings);

public:
    /// velocity from imu sensors
    XInputPort<xcs::CartesianVector> measuredVelocity;
    /// rotation from imu sensors
    XInputPort<xcs::EulerianVector> measuredRotation;
    /// quadricopter altitude 
    XInputPort<double> measuredAltitude;
    /// time when all imu measurement have been measured
    XInputPort<double> timeImu;
    /// video from quadricopter front camera
    XInputPort<::urbi::UImage> video;
    /// time when video have been captured
    XInputPort<double> videoTime;
    /// quadricopter fly control
    XInputPort<xcs::FlyControl> flyControl;
    /// time when fly control takes effect on quadricopter
    XInputPort<double> flyControlSendTime;
    /// PTAM and EKF control commands
    XInputPort<std::string> control;
    /// Enable and disable ptam framework
    XInputPort<bool> ptamEnabled;
    /// Set localization position
    XInputPort<xcs::CartesianVector> setPosition;
    /// Set localization rotation
    XInputPort<xcs::EulerianVector> setRotation;

    /// Computed quadricopter position
    XVar<xcs::CartesianVector> position;
    /// Computed quadricopter velocity
    XVar<xcs::CartesianVector> velocity;
    /// Computed quadricopter rotation
    XVar<xcs::EulerianVector> rotation;
    /// Computed quadricopter rotation velocity in z axis
    XVar<double> velocityPsi;
    /// PTAM status
    XVar<int> ptamStatus;

    /// Initialize private variables
    XLocalization(const std::string &name);

    /*! Load and apply localization settings from configuration file on XLocalization

        \param congifFile absolute path of the XLocalization configuration file
    */
    void init(const std::string &configFile);
};

}
}

#endif