#include "AT_Command.hpp"
#include "../../shared/basic.hpp"

droneMove::droneMove(float roll, float pitch, float yaw, float gaz){
		this->roll.floatNumber = valueInRange(roll,1.0f);
		this->pitch.floatNumber = valueInRange(pitch,1.0f);
		this->yaw.floatNumber = valueInRange(yaw,1.0f);
		this->gaz.floatNumber = valueInRange(gaz,1.0f);
}