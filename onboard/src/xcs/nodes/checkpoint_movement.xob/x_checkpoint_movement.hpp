#ifndef XCHECKPOINT_MOVEMENT_H
#define XCHECKPOINT_MOVEMENT_H

#include "checkpoint_movement.hpp"

#include <xcs/types/checkpoint.hpp>
#include <xcs/types/cartesian_vector.hpp>
#include <xcs/types/eulerian_vector.hpp>
#include <xcs/types/velocity_control.hpp>

#include <xcs/nodes/xobject/x_object.hpp>
#include <xcs/nodes/xobject/x_input_port.hpp>
#include <xcs/nodes/xobject/x_var.hpp>

namespace xcs {
namespace nodes {

class XCheckpointMovement : public xcs::nodes::XObject {
    xcs::nodes::checkpoint::CheckpointMovement CheckpointMovement_;

    std::atomic<bool> stopped_;

    void callback(bool reached);

    void onChangeCommand(std::string command);
    // Programmer have to ensure calling next three function in proper order (position,velocity,rotation) and together!!!
    void onChangeAddCheckpoint(xcs::Checkpoint checkpoint);
    void onChangeDronePosition(xcs::CartesianVector position);
    // Set droneRotation and compute next point on way to the first checkpoint
    void onChangeDroneRotation(xcs::EulerianVector rotation);
protected:
    virtual void stateChanged(XObject::State state);
public:
    /// serve for passing command like delete all items from checkpoint queue etc.
    xcs::nodes::XInputPort<std::string> command; 
    /// add checkpoint in checkpoints queue for quadricopter flight
    xcs::nodes::XInputPort<xcs::Checkpoint> checkpoint;
    /// insert actual quadricopter position
    xcs::nodes::XInputPort<xcs::CartesianVector> dronePosition;
    /// insert actual quadricopter rotation
    xcs::nodes::XInputPort<xcs::EulerianVector> droneRotation;
    /// publish quadricopter velocity control for reaching desired checkpoints
    xcs::nodes::XVar<xcs::VelocityControl> velocityControl;
    /// whether quadricopter reached first checkpoint from checkpoint queue
    xcs::nodes::XVar<bool> reachedCheckpoint;

    /// Initialize private variable and register input ports and xvars in Urbi
    XCheckpointMovement(const std::string& name);
};

}
}

#endif