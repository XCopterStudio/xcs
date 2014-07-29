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

    typedef double PID_TYPE;
    typedef double RETURN_TYPE;
    typedef xcs::PID<PID_TYPE, RETURN_TYPE> PID;

    class Control{
        static const double MAX_VALUE_ROLL_PITCH;
        static const double MAX_VALUE_YAW_GAZ;

        // PID
        PID vxPID_;
        PID vyPID_;
        PID vzPID_;
        PID psiPID_;

        // Drone state
        xcs::CartesianVector velocity_;
        xcs::EulerianVector rotation_;

        // Drone desire speed
        xcs::SpeedControl desireVelocity_;
        
    public:
        Control();
        // Programmer have to assure that firstly change velocity, rotation and immediately after that call computeControl!!!
        void velocity(const xcs::CartesianVector &velocity);
        void rotation(const xcs::EulerianVector &rotation);

        void desireVelocity(const xcs::SpeedControl &speedControl);

        xcs::FlyControl computeControl();

        inline void setVxPID(PID_TYPE P, PID_TYPE I, PID_TYPE D){ vxPID_.P(P); vxPID_.I(I); vxPID_.D(D);  };
        inline void setVyPID(PID_TYPE P, PID_TYPE I, PID_TYPE D){ vyPID_.P(P); vyPID_.I(I); vyPID_.D(D); };;
        inline void setVzPID(PID_TYPE P, PID_TYPE I, PID_TYPE D){ vzPID_.P(P); vzPID_.I(I); vzPID_.D(D); };;
        inline void setPsiPID(PID_TYPE P, PID_TYPE I, PID_TYPE D){ psiPID_.P(P); psiPID_.I(I); psiPID_.D(D); };;
    };

}}}

#endif