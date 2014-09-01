#ifndef X_FLYCONTROL_AGGREGATOR_H
#define X_FLYCONTROL_AGGREGATOR_H

#include <xcs/nodes/xobject/x_object.hpp>
#include <xcs/nodes/xobject/x_input_port.hpp>
#include <xcs/nodes/xobject/x_var.hpp>

#include <xcs/types/fly_control.hpp>

#include <atomic>

namespace xcs{
namespace nodes{
namespace aggregator{

    class XFlyControlAggregator : public XObject{
        FlyControl flyControl_;

        std::atomic<bool> stopped_;

        void onChangeRoll(const double roll);
        void onChangePitch(const double pitch);
        void onChangeYaw(const double yaw);
        void onChangeGaz(const double gaz);
    protected:
        virtual void stateChanged(XObject::State state);
    public:
        XInputPort<double> roll;
        XInputPort<double> pitch;
        XInputPort<double> yaw;
        XInputPort<double> gaz;

        XVar<FlyControl> flyControl;

        XFlyControlAggregator(const std::string& name);
    };

}}}

#endif 

