#ifndef X_PID_H
#define X_PID_H

#include <xcs/nodes/xobject/x_object.hpp>
#include <xcs/nodes/xobject/x_input_port.hpp>
#include <xcs/nodes/xobject/x_var.hpp>

#include <xcs/pid.hpp>

namespace xcs{
namespace nodes{
namespace pid{

    class XPID : public XObject{
        typedef double PidType;
        PID<PidType,double> pid_;
        double desireValue_;

        void onChangeP(const PidType p);
        void onChangeI(const PidType i);
        void onChangeD(const PidType d);
        void onChangeActualValue(const double actualValue);
        void onChangeDesireValue(const double desireValue);
    public:
        XInputPort<PidType> P;
        XInputPort<PidType> I;
        XInputPort<PidType> D;

        XInputPort<double> desireValue;
        XInputPort<double> actualValue;

        XVar<double> control;

        XPID(const std::string& name);
    };

}}}

#endif