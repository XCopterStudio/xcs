#include "xhermit_movement.hpp"
#include <functional>

using namespace xcs::nodes;

void XHermitMovement::onChangeCommand(std::string command){
    if (command == "ClearCheckpoint"){
        hermitMovement_.deleteCheckpoints();
    }
}

void XHermitMovement::onChangeAddCheckpoint(xcs::Checkpoint checkpoint){
    hermitMovement_.addCheckpoint(checkpoint);
}

void XHermitMovement::onChangeDronePosition(xcs::CartesianVector position){
    hermitMovement_.dronePosition(position);
}

void XHermitMovement::onChangeDroneRotation(xcs::EulerianVector rotation){
    hermitMovement_.droneRotation(rotation);
    if (!stopped_){
        speedControl = hermitMovement_.flyOnCheckpoint();
    }
}

void XHermitMovement::stateChanged(XObject::State state) {
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
XHermitMovement::XHermitMovement(const std::string& name) : XObject(name) ,
hermitMovement_(boost::bind(&XHermitMovement::callbackHermit, this, _1)),
command("COMMAND"),
checkpoint("CHECKPOINT"),
dronePosition("POSITION_ABS"),
droneRotation("ROTATION"),
speedControl("SPEED_CONTROL_ABS"),
reachedCheckpoint("BOOL")
{
    XBindVarF(command, &XHermitMovement::onChangeCommand);
    XBindVarF(checkpoint, &XHermitMovement::onChangeAddCheckpoint);
    XBindVarF(dronePosition, &XHermitMovement::onChangeDronePosition);
    XBindVarF(droneRotation, &XHermitMovement::onChangeDroneRotation);
    XBindVar(speedControl);
    XBindVar(reachedCheckpoint);

    stopped_ = true;
    reachedCheckpoint = false;
}

void XHermitMovement::callbackHermit(bool reached){
    reachedCheckpoint = reached;
}

XStart(XHermitMovement);