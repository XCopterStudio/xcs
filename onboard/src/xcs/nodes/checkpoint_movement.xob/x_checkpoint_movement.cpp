#include "x_checkpoint_movement.hpp"
#include <functional>

using namespace xcs::nodes;

void XCheckpointMovement::onChangeCommand(std::string command){
    if (command == "ClearCheckpoint"){
        CheckpointMovement_.deleteCheckpoints();
    }
}

void XCheckpointMovement::onChangeAddCheckpoint(xcs::Checkpoint checkpoint){
    CheckpointMovement_.addCheckpoint(checkpoint);
}

void XCheckpointMovement::onChangeDronePosition(xcs::CartesianVector position){
    CheckpointMovement_.dronePosition(position);
}

void XCheckpointMovement::onChangeDroneRotation(xcs::EulerianVector rotation){
    CheckpointMovement_.droneRotation(rotation);
    if (!stopped_){
        speedControl = CheckpointMovement_.flyOnCheckpoint();
    }
}

void XCheckpointMovement::stateChanged(XObject::State state) {
    switch (state) {
        case XObject::STATE_STARTED:
            stopped_ = false;
            break;
        case XObject::STATE_STOPPED:
            stopped_ = true;
            break;
    }
}

//==================== public functions =============
XCheckpointMovement::XCheckpointMovement(const std::string& name) : XObject(name) ,
CheckpointMovement_(boost::bind(&XCheckpointMovement::callbackHermit, this, _1)),
command("COMMAND"),
checkpoint("CHECKPOINT"),
dronePosition("POSITION_ABS"),
droneRotation("ROTATION"),
speedControl("SPEED_CONTROL_ABS"),
reachedCheckpoint("BOOL")
{
    XBindVarF(command, &XCheckpointMovement::onChangeCommand);
    XBindVarF(checkpoint, &XCheckpointMovement::onChangeAddCheckpoint);
    XBindVarF(dronePosition, &XCheckpointMovement::onChangeDronePosition);
    XBindVarF(droneRotation, &XCheckpointMovement::onChangeDroneRotation);
    XBindVar(speedControl);
    XBindVar(reachedCheckpoint);

    stopped_ = true;
    reachedCheckpoint = false;
}

void XCheckpointMovement::callbackHermit(bool reached){
    reachedCheckpoint = reached;
}

XStart(XCheckpointMovement);