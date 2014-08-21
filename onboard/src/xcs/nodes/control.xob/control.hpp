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

    /*! \brief Control quadricopter velocity based on desire quadricopter velocity and actual quadricopter state.
    
        Desire and actual quadricopter velocity have to be in m/s and global frame.
        Quadricopter rotation have to be in radians.
        \warning Programmer have to assure that firstly change velocity, rotation and immediately after that call computeControl!!!
    */
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
        /*! Initialize private PID variables. */
        Control();
        
        /*! Set actual velocity of an quadricopter.

            It have to be call first for every update step and before rotation()
            \param velocity actual velocity of an quadricopter in m/s.
        */
        void velocity(const xcs::CartesianVector &velocity);
        /*! Set actual rotation of an quadricopter.
            
            It have to be call after velocity() and before computeControl()
            \param rotation actual rotation of an quadricopter in radians.
        */
        void rotation(const xcs::EulerianVector &rotation);
        /*! Set desire velocity which Control try keep with an quadricopter
        
            \param speedControl desire speed an quadricopter in m/s
        */
        void desireVelocity(const xcs::SpeedControl &speedControl);

        /*! Compute FlyControl for an quadricopter based on actual state and desire velocity.
        
            \return xcs::FlyControl for an quadricopter control
        */
        xcs::FlyControl computeControl();

        /*! Setter for PID parameters.
            
            \param P proportional parameter 
            \param I integral parameter
            \param D derivative parameter
        */
        inline void setVxPID(PID_TYPE P, PID_TYPE I, PID_TYPE D){ vxPID_.P(P); vxPID_.I(I); vxPID_.D(D);  };
        /*! Setter for PID parameters.

        \param P proportional parameter
        \param I integral parameter
        \param D derivative parameter
        */
        inline void setVyPID(PID_TYPE P, PID_TYPE I, PID_TYPE D){ vyPID_.P(P); vyPID_.I(I); vyPID_.D(D); };
        /*! Setter for PID parameters.

        \param P proportional parameter
        \param I integral parameter
        \param D derivative parameter
        */
        inline void setVzPID(PID_TYPE P, PID_TYPE I, PID_TYPE D){ vzPID_.P(P); vzPID_.I(I); vzPID_.D(D); };
        /*! Setter for PID parameters.

        \param P proportional parameter
        \param I integral parameter
        \param D derivative parameter
        */
        inline void setPsiPID(PID_TYPE P, PID_TYPE I, PID_TYPE D){ psiPID_.P(P); psiPID_.I(I); psiPID_.D(D); };
    };

}}}

#endif