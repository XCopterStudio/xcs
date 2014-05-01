#ifndef CONTROL_H
#define CONTROL_H

#include <xcs/types/fly_control.hpp>
#include <xcs/types/cartesian_vector.hpp>
#include <xcs/types/eulerian_vector.hpp>
#include <xcs/types/speed_control.hpp>
#include "pid.hpp"

namespace xcs{
namespace nodes{
namespace control{

    class Control{
        static const double MAX_VALUE;

        // PID
        xcs::PID<int, double> vxPID_;
        xcs::PID<int, double> vyPID_;
        xcs::PID<int, double> vzPID_;
        xcs::PID<int, double> psiPID_;

        // Drone state
        xcs::CartesianVector velocity_;
        xcs::EulerianVector rotation_;

        // Drone desire speed
        xcs::SpeedControl desireSpeed_;

        xcs::FlyControl computeControl();
        
    public:
        Control();
        // Programmer have to assure that firstly change velocity and immediately after that change rotation!!!
        void updateVelocity(const xcs::CartesianVector &velocity);
        xcs::FlyControl updateRotation(const xcs::EulerianVector &rotation);

        void updateSpeedControl(const xcs::SpeedControl &speedControl);
    };

}}}

#endif