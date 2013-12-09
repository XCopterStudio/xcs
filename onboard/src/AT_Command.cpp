#include "AT_Command.hpp"
#include "../../shared/basic.hpp"

#include <sstream>

//remove
#include <iostream>
//end remove

//! General constants for creating at commands.
static const std::string startOfAtComm = "AT*";
static const std::string endOfAtComm = "\r";
static const std::string delOfAtCommVal = ",";

//! Specific names for all at commands
const std::string atCommandRef::nameOfCommand = "REF";
const std::string atCommandPCMD::nameOfCommand = "PCMD";
const std::string atCommandPCMD_MAG::nameOfCommand = "PCMD_MAG";
const std::string atCommandFTRIM::nameOfCommand = "FTRIM";
const std::string atCommandCALIB::nameOfCommand = "CALIB";
const std::string atCommandCONFIG::nameOfCommand = "CONFIG";
const std::string atCommandCONFIG_IDS::nameOfCommand = "CONFIG_IDS";
const std::string atCommandCOMWDG::nameOfCommand = "COMWDG";

droneMove::droneMove(float roll, float pitch, float yaw, float gaz){
		this->roll.floatNumber = valueInRange(roll,1.0f);
		this->pitch.floatNumber = valueInRange(pitch,1.0f);
		this->yaw.floatNumber = valueInRange(yaw,1.0f);
		this->gaz.floatNumber = valueInRange(gaz,1.0f);
}

std::string atCommandRef::toString(const unsigned int sequenceNumber){
		// create magic bitField for basic behavior (take off, land, etc.)
	// from documentation this bits must be set to 1
	int bitField = (1 << 18) | (1 << 20) | (1 << 22) | (1 << 24) |(1 << 28);
	switch(basicBehavior){
		case TAKEOFF:
			bitField |= (1 << 9); // 9-bit of bit field decides between take-off and land, 1 for take off, 0 for land
			break;
		case LAND:
			bitField &= ~(1 << 9); // 9-bit of bit field decides between take-off and land, 1 for take off, 0 for land
			break;
		case EMERGENCY:
			bitField |= (1 << 8); // when drone is in normal state this bit set to 1 makes the drones enter to an emergency mode (cut off engine).
			break;
		case NORMAL:
			bitField |= (1 << 8); // when drone is in emergency state this bit set to 1 makes the drones resume a normal mode.
		case CLEAR:
			// don't change anything on bit field
			break;
		default: // this state is not accessible
			break;
	}

	std::stringstream out;
	out << startOfAtComm << nameOfCommand << "=" << sequenceNumber;
	out << delOfAtCommVal;
	out << bitField;
	out << endOfAtComm;

	return out.str();
}

atCommandPCMD::atCommandPCMD(const droneMove movement,const bool absControl, const bool combYaw,const bool progCmd) : move(movement), absoluteControl(absControl), combinedYaw(combYaw), progressivCommand(progCmd){

}

std::string atCommandPCMD::toString(const unsigned int sequenceNumber){
	int bitField = 0;
	// Create bitField according to Boolean arguments. More details in ar drone documentation.
	bitField |= (progressivCommand << 0);
	bitField |= (combinedYaw << 1);
	bitField |= (absoluteControl << 2);

	std::stringstream out;
	out << startOfAtComm << nameOfCommand << "=" << sequenceNumber;
	out << delOfAtCommVal << bitField;
	out << delOfAtCommVal << move.roll.intNumber;
	out << delOfAtCommVal << move.pitch.intNumber;
	out << delOfAtCommVal << move.gaz.intNumber;
	out << delOfAtCommVal << move.yaw.intNumber;
	out << endOfAtComm;

	return out.str();
}

atCommandPCMD_MAG::atCommandPCMD_MAG(const droneMove movement,const float psi, const float psiAccur, const bool absControl, const bool combYaw, const bool progCmd) : atCommandPCMD(movement), magnetoAngle(psi), magnetoAccuracy(psiAccur){

}

std::string atCommandPCMD_MAG::toString(const unsigned int sequenceNumber){
	std::string pcmd = atCommandPCMD::toString(sequenceNumber);

	std::stringstream out;
	// remove last character (\r) from pcmd command
	out << pcmd.substr(0, pcmd.length() -1);
	// append additional values of drone heading according to magnetometer 
	out << delOfAtCommVal << magnetoAngle.intNumber;
	out << delOfAtCommVal << magnetoAccuracy.intNumber;
	out << endOfAtComm;

	return out.str();
}

void main(){
	std::cout << atCommandPCMD_MAG(droneMove(), 0.1f,0.1f).toString(1);
}