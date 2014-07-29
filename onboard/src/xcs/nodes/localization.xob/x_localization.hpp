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
    TimePoint lastFrameTime_;
    std::atomic<bool> ptamEnabled_;

    void onChangeVelocity(xcs::CartesianVector measuredVelocity);
    void onChangeRotation(xcs::EulerianVector measuredAnglesRotation);
    void onChangeAltitude(double altitude);
    // change time after you change all imu measurements (velocity etc.) 
    // because this add new imu measurement in ekf queue
    void onChangeTimeImu(double internalTime);

    void onChangeVideo(::urbi::UImage image);
    void onChangeVideoTime(xcs::Timestamp internalTime); // TODE deprecated use onChanheTimeCam or vice versa

    void onChangeFlyControl(const xcs::FlyControl flyControl);
    void onChangeFlyControlSendTime(const double flyControlSendTime);

    void onChangeControl(const std::string &control);
    
    void onChangePtamEnabled(const bool ptamEnabled);

    inline double timeFromStart() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
                clock_.now() - startTime_).count() / 1000.0;
    };

    void loadParameters(const xcs::Settings &settings);

public:
    // imu measurements
    XInputPort<xcs::CartesianVector> measuredVelocity;
    XInputPort<xcs::EulerianVector> measuredAnglesRotation; // TODO rename
    XInputPort<double> measuredAltitude;
    XInputPort<double> timeImu;
    // visual data
    XInputPort<::urbi::UImage> video;
    XInputPort<double> videoTime;
    // drone fly control
    XInputPort<xcs::FlyControl> flyControl;
    XInputPort<double> flyControlSendTime;
    // PTAM control
    XInputPort<std::string> control;
    XInputPort<bool> ptamEnabled;

    // computed ekf output
    XVar<xcs::CartesianVector> position;
    XVar<xcs::CartesianVector> velocity;
    XVar<xcs::EulerianVector> rotation;
    XVar<double> velocityPsi;
    // PTAM status
    XVar<int> ptamStatus;

    XLocalization(const std::string &name);

    void init(const std::string &configFile);
};

}
}

#endif