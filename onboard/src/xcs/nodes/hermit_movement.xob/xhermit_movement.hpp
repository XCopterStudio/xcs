#ifndef XHERMIT_MOVEMENT_H
#define XHERMIT_MOVEMENT_H

#include "hermit_movement.hpp"

#include <xcs/types/checkpoint.hpp>
#include <xcs/types/cartesian_vector.hpp>
#include <xcs/types/eulerian_vector.hpp>
#include <xcs/types/speed_control.hpp>

#include <xcs/nodes/xobject/x_object.hpp>
#include <xcs/nodes/xobject/x_input_port.hpp>
#include <xcs/nodes/xobject/x_var.hpp>

namespace xcs {
namespace nodes {

    class XHermitMovement : public xcs::nodes::XObject{
        xcs::nodes::hermit::HermitMovement hermitMovement_;

        void onChangeCommand(std::string command);
        // Programmer have to ensure calling next three function in proper order (position,velocity,rotation) and together!!!
        void onChangeAddCheckpoint(xcs::Checkpoint checkpoint);
        void onChangeDronePosition(xcs::CartesianVector position);
        // Set droneRotation and compute next hermit point on way to the first checkpoint
        void onChangeDroneRotation(xcs::EulerianVector rotation);
    public:
        // input ports
        xcs::nodes::XInputPort<std::string> command; // serve for passing command like delete all items from checkpoint queue etc.
        xcs::nodes::XInputPort<xcs::Checkpoint> checkpoint;
        xcs::nodes::XInputPort<xcs::CartesianVector> dronePosition;
        xcs::nodes::XInputPort<xcs::EulerianVector> droneRotation;
        // xvars
        xcs::nodes::XVar<xcs::SpeedControl> speedControl;

        XHermitMovement(const std::string& name);
    };

}
}

#endif