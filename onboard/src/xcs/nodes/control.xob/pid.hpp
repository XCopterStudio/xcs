#ifndef PID_H
#define PID_H

namespace xcs{

    template <class pid_type, class value_type>
    class PID{
        pid_type P_;
        pid_type I_;
        pid_type D_;

        value_type sumError_;
        value_type lastValue_;
    public:
        PID(const pid_type p = 1, const pid_type i = 0, const pid_type d = 0);

        inline void P(const pid_type &p){ P_ = p; };
        inline void I(const pid_type &i){ I_ = i; };
        inline void D(const pid_type &d){ D_ = d; };
        
        value_type getCorrection(const value_type &actualValue, const value_type &desireValue);
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

}

#endif