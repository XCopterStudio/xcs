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

    class XEkf : public XObject{
        static const double IMU_DELAY;
        static const double FLY_CONTROL_SEND_TIME;

        ekf::Ekf ekf_;
        double lastAltitude_;
        ekf::DroneStateMeasurement lastMeasurement_;
        double lastMeasurementTime_;

        Clock clock_;
        TimePoint startTime_;
        double imuTimeShift_;

        void onChangeVelocity(xcs::CartesianVector measuredVelocity);
        void onChangeRotation(xcs::EulerianVector measuredAnglesRotation);
        void onChangeAltitude(double altitude);
        // change time after you change all imu measurements (velocity etc.) 
        // because this add new imu measurement in ekf queue
        void onChangeTimeImu(double timeImu); 
        void onChangeFlyControl(xcs::FlyControl flyControl);

        inline double timeFromStart(){ return std::chrono::duration_cast<std::chrono::milliseconds>(
            clock_.now() - startTime_).count() / 1000.0; };
    public:
        // imu measurements
        XInputPort<xcs::CartesianVector> measuredVelocity;
        XInputPort<xcs::EulerianVector> measuredAnglesRotation;
        XInputPort<double> measuredAltitude;
        XInputPort<double> timeImu;
        // drone fly control
        XInputPort<xcs::FlyControl> flyControl;

        // computed ekf output
        XVar<xcs::CartesianVector> position;
        XVar<xcs::CartesianVector> velocity;
        XVar<xcs::EulerianVector> rotation;

        XEkf(const std::string &name);
    };
    
}}

#endif