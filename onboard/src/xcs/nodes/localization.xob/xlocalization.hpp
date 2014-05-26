#ifndef XEKF_H
#define XEKF_H

#include <chrono>

#include "ekf.hpp"
#include <xcs/xcs_fce.hpp>

#include <xcs/nodes/xobject/x_object.hpp>
#include <xcs/nodes/xobject/x_input_port.hpp>
#include <xcs/nodes/xobject/x_var.hpp>

#include <xcs/types/cartesian_vector.hpp>
#include <xcs/types/eulerian_vector.hpp>

namespace xcs{
namespace nodes{

    typedef std::chrono::high_resolution_clock Clock;
    typedef std::chrono::time_point<std::chrono::high_resolution_clock> TimePoint;

    class XLocalization : public XObject{
        static const double IMU_DELAY;
        static const double FLY_CONTROL_SEND_TIME;
        static const double CAM_DELAY;

        localization::Ekf ekf_;

        localization::DroneStateMeasurement lastMeasurement_;
        double lastMeasurementTime_;

        localization::CameraMeasurement lastCameraMeasurement_;

        Clock clock_;
        TimePoint startTime_;
        double imuTimeShift_;

        void onChangeVelocity(xcs::CartesianVector measuredVelocity);
        void onChangeRotation(xcs::EulerianVector measuredAnglesRotation);
        void onChangeAltitude(double altitude);
        // change time after you change all imu measurements (velocity etc.) 
        // because this add new imu measurement in ekf queue
        void onChangeTimeImu(double timeImu); 

        void onChangePosition(xcs::CartesianVector measuredPosition);
        void onChangeAngles(xcs::EulerianVector measuredAngles);
        void onChangeTimeCam(double timeCam);

        void onChangeClearTime(double time);

        void onChangeFlyControl(xcs::FlyControl flyControl);

        inline double timeFromStart(){ return std::chrono::duration_cast<std::chrono::milliseconds>(
            clock_.now() - startTime_).count() / 1000.0; };
    public:
        // imu measurements
        XInputPort<xcs::CartesianVector> measuredVelocity;
        XInputPort<xcs::EulerianVector> measuredAnglesRotation;
        XInputPort<double> measuredAltitude;
        XInputPort<double> timeImu;
        // cam measurements
        XInputPort<xcs::CartesianVector> measuredPosition;
        XInputPort<xcs::EulerianVector> measuredAngles;
        XInputPort<double> timeCam;
        // clear channel
        XInputPort<double> clearTime;
        // drone fly control
        XInputPort<xcs::FlyControl> flyControl;

        // computed ekf output
        XVar<xcs::CartesianVector> position;
        XVar<xcs::CartesianVector> velocity;
        XVar<xcs::EulerianVector> rotation;

        XLocalization(const std::string &name);
    };
    
}}

#endif