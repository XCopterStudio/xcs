#ifndef XCONTROL_H
#define XCONTROL_H

#include "control.hpp"

#include <xcs/types/eulerian_vector.hpp>
#include <xcs/types/cartesian_vector.hpp>
#include <xcs/types/fly_control.hpp>
#include <xcs/types/speed_control.hpp>

#include <xcs/nodes/xobject/x_object.hpp>
#include <xcs/nodes/xobject/x_input_port.hpp>
#include <xcs/nodes/xobject/x_var.hpp>

namespace xcs{
namespace nodes{
namespace control{

    class XControl : public XObject{
        Control control_;

        void onChangeVelocity(xcs::CartesianVector velocity);
        void onChangeRotation(xcs::EulerianVector rotation);
        void onChangeDesireSpeed(xcs::SpeedControl desireSpeed);
    public:
        // Incoming data
        xcs::nodes::XInputPort<xcs::CartesianVector> velocity;
        xcs::nodes::XInputPort<xcs::EulerianVector> rotation;
        xcs::nodes::XInputPort<xcs::SpeedControl> desireSpeed;
        // Output data
        xcs::nodes::XVar<xcs::FlyControl> flyControl;

        XControl(const std::string& name);
    };

}}}

#endif