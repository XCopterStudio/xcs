#ifndef X_LOCALIZATION_H
#define X_LOCALIZATION_H

#include <chrono>
#include <memory>
#include <mutex>

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

    localization::Ekf ekf_;

    PtamPtr ptam_;

    localization::DroneStateMeasurement lastMeasurement_;
    double lastMeasurementTime_;

    
    Clock clock_;
    TimePoint startTime_;
    double imuTimeShift_;
    ::urbi::UImage lastFrame_;
    std::mutex lastFrameMtx_;

    void onChangeVelocity(xcs::CartesianVector measuredVelocity);
    void onChangeRotation(xcs::EulerianVector measuredAnglesRotation);
    void onChangeAltitude(double altitude);
    // change time after you change all imu measurements (velocity etc.) 
    // because this add new imu measurement in ekf queue
    void onChangeTimeImu(double internalTime);

    void onChangeVideo(::urbi::UImage image);
    void onChangeVideoTime(xcs::Timestamp internalTime); // TODE deprecated use onChanheTimeCam or vice versa

    void onChangeFlyControl(const xcs::FlyControl flyControl);
    
    void onChangePtamControl(const std::string &ptamControl);

    inline double timeFromStart() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
                clock_.now() - startTime_).count() / 1000.0;
    };
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
    // PTAM control
    XInputPort<std::string> ptamControl;

    // computed ekf output
    XVar<xcs::CartesianVector> position;
    XVar<xcs::CartesianVector> velocity;
    XVar<xcs::EulerianVector> rotation;
    XVar<double> velocityPsi;

    XLocalization(const std::string &name);
    
    void init();
    void loadParameters(const std::string &file);
};

}
}

#endif