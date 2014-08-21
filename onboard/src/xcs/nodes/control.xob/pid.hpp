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
            \param i
            \param d
        */
        PID(const pid_type p = 1, const pid_type i = 0, const pid_type d = 0);

        inline void P(const pid_type &p){ P_ = p; };
        inline void I(const pid_type &i){ I_ = i; };
        inline void D(const pid_type &d){ D_ = d; };
        
        value_type getCorrection(const value_type &actualValue, const value_type &desireValue);
        value_type getCorrection(const value_type &error);
    };

    template <class pid_type, class value_type>
    PID<pid_type,value_type>::PID(const pid_type p, const pid_type i, const pid_type d) : 
        P_(p),
        I_(i),
        D_(d)
    {
        sumError_ = 0;
        lastValue_ = 0;
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