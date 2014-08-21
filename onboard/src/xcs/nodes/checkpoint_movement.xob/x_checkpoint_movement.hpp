#ifndef XCHECKPOINT_MOVEMENT_H
#define XCHECKPOINT_MOVEMENT_H

#include "checkpoint_movement.hpp"

#include <xcs/types/checkpoint.hpp>
#include <xcs/types/cartesian_vector.hpp>
#include <xcs/types/eulerian_vector.hpp>
#include <xcs/types/speed_control.hpp>

#include <xcs/nodes/xobject/x_object.hpp>
#include <xcs/nodes/xobject/x_input_port.hpp>
#include <xcs/nodes/xobject/x_var.hpp>

namespace xcs {
namespace nodes {

class XCheckpointMovement : public xcs::nodes::XObject {
    xcs::nodes::hermit::CheckpointMovement CheckpointMovement_;

    std::atomic<bool> stopped_;

    void onChangeCommand(std::string command);
    // Programmer have to ensure calling next three function in proper order (position,velocity,rotation) and together!!!
    void onChangeAddCheckpoint(xcs::Checkpoint checkpoint);
    void onChangeDronePosition(xcs::CartesianVector position);
    // Set droneRotation and compute next hermit point on way to the first checkpoint
    void onChangeDroneRotation(xcs::EulerianVector rotation);
protected:
    virtual void stateChanged(XObject::State state);
public:
    // input ports
    xcs::nodes::XInputPort<std::string> command; // serve for passing command like delete all items from checkpoint queue etc.
    xcs::nodes::XInputPort<xcs::Checkpoint> checkpoint;
    xcs::nodes::XInputPort<xcs::CartesianVector> dronePosition;
    xcs::nodes::XInputPort<xcs::EulerianVector> droneRotation;
    // xvars
    xcs::nodes::XVar<xcs::SpeedControl> speedControl;
    xcs::nodes::XVar<bool> reachedCheckpoint;

    void callbackHermit(bool reached);

    XCheckpointMovement(const std::string& name);
};

}
}

#endif