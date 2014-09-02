#ifndef PID_H
#define PID_H

namespace xcs{

    /*! \brief PID controller. 
    
        \sa http://en.wikipedia.org/wiki/PID_controller
    */
    template <class pid_type, class value_type>
    class PID{
        pid_type P_;
        pid_type I_;
        pid_type D_;

        value_type sumError_;
        value_type lastValue_;
    public:
        /*! Initialize parameters of the PID controller.
        
            \param p Proportional parameter
            \param i Integral parameter
            \param d Derivative parameter
        */
        PID(const pid_type p = 1, const pid_type i = 0, const pid_type d = 0);

        /// Set proportional parameter
        inline void P(const pid_type &p){ P_ = p; };
        /// Set integral parameter
        inline void I(const pid_type &i){ I_ = i; };
        /// Set derivative parameter
        inline void D(const pid_type &d){ D_ = d; };
        
        /*! Compute controller output.
            
            \param actualValue actual value of controlled process
            \param desireValue desire value of controlled process
            \return control value for process
        */
        value_type getCorrection(const value_type &actualValue, const value_type &desireValue);
        /*! Compute controller output.

        \param error actual error of controlled process
        \return control value for process
        */
        value_type getCorrection(const value_type &error);
    };

    template <class pid_type, class value_type>
    PID<pid_type,value_type>::PID(const pid_type p, const pid_type i, const pid_type d) : 
        P_(p),
        I_(i),
        D_(d)
    {
    }

    template <class pid_type, class value_type>
    value_type PID<pid_type, value_type>::getCorrection(const value_type &actualValue, const value_type &desireValue){
        value_type error = desireValue - actualValue;
        value_type valueChange = actualValue - lastValue_;
        sumError_ += error;
        lastValue_ = actualValue;
        return P_*error + I_*sumError_ + D_*valueChange;
    }

    template <class pid_type, class value_type>
    value_type PID<pid_type, value_type>::getCorrection(const value_type &error){
        sumError_ += error;
        return P_*error + I_*sumError_;
    }

}

#endif