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
        xcs::PID<double, double> vxPID_;
        xcs::PID<double, double> vyPID_;
        xcs::PID<double, double> vzPID_;
        xcs::PID<double, double> psiPID_;

        // Drone state
        xcs::CartesianVector velocity_;
        xcs::EulerianVector rotation_;

        // Drone desire speed
        xcs::SpeedControl desireSpeed_;

        
        
    public:
        Control();
        // Programmer have to assure that firstly change velocity, rotation and immediately after that call computeControl!!!
        void velocity(const xcs::CartesianVector &velocity);
        void rotation(const xcs::EulerianVector &rotation);

        void desireSpeed(const xcs::SpeedControl &speedControl);

        xcs::FlyControl computeControl();
    };

}}}

#endif