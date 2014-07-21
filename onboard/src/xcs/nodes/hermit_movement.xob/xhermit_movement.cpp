#include "xhermit_movement.hpp"
#include <functional>

using namespace xcs::nodes;

void XHermitMovement::onChangeCommand(std::string command){
    if (command == "ClearCheckpoint"){
        hermitMovement_.deleteCheckpoints();
    }
    else{
        
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
    speedControl = hermitMovement_.flyOnCheckpoint();
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
}

void XHermitMovement::callbackHermit(bool reached){
    reachedCheckpoint = reached;
}

XStart(XHermitMovement);