#include "AT_Command.hpp"
#include "../../shared/basic.hpp"

droneMove::droneMove(float roll, float pitch, float yaw, float gaz) 
	: roll(valueInRange(roll,1.0f)), pitch(valueInRange(pitch,1.0f))
	, yaw(valueInRange(yaw,1.0f)), gaz(valueInRange(gaz,1.0f)){
}