#ifndef AT_COMMAND_H
#define AT_COMMAND_H

#include <string>
#include <cstdint>

//! Basic behavior of parrot drone
enum behavior{
	TAKEOFF,
	LAND,
	EMERGENCY,
	NORMAL
};

//! Serve for easier conversion between 32-bit integer and float

//! Use this union for preserving float values of an at command, because parrot
//! expect this values written in at command as 32bit integer.
union float_int{
	int32_t intNumber;
	float floatNumber;

	float_int(int32_t intNumber) : intNumber(intNumber) {};
	float_int(float floatNumber = 0.0f) : floatNumber(floatNumber) {};
};

//! Drone's constant representation of movement. 

//! All input value must be in range <-1,1> otherwise constructor and setter 
//! corect input value to this range. All values lower or higher than limits
//! will be replaced with this limits. 
const struct droneMove{
	float_int roll;
	float_int pitch;
	float_int yaw;
	float_int gaz;

	droneMove(float roll = 0, float pitch = 0, float yaw = 0, float gaz = 0);
};

//! Basic class representing at command from which all at commands must be inherited.

//! At command class serve only as general pointer to the inherited commands and printing.
class atCommand{
public:
	virtual std::string toString(const unsigned int sequenceNumber) = 0;
};

//! Drone take off, landing and emergency stop command.
class atCommandRef : public atCommand{
	const behavior basicBehavior;
public:
	atCommandRef(const behavior basicBehavior) : basicBehavior(basicBehavior) {};
	std::string toString(const unsigned int sequenceNumber);
};

//! Move drone command.
class atCommandPCMD : public atCommand{
protected:
	droneMove move;
	bool absoluteControl;
	bool combinedYaw;
	bool progressivCommand;
public:
	std::string toString(const unsigned int sequenceNumber);
};

//! Move drone command with absolute control support. 
class atCommandPCMD_MAG : public atCommandPCMD{
	float_int magnetoAngle;
	float_int magnetoAccuracy;
public:
	std::string toString(const unsigned int sequenceNumber);
};

//! Sets the reference for horizontal plane. Must be on the ground.
class atCommandFTRIM : public atCommand{
public:
	std::string toString(const unsigned int sequenceNumber);
};

//! Configuration of the drone.
class atCommandCONFIG : public atCommand{
public:
	std::string toString(const unsigned int sequenceNumber);
};

//! Identifiers for atCommandConfig.
class atCommandCONFIG_IDS : public atCommand{
public:
	std::string toString(const unsigned int sequenceNumber);
};

//! Reset the communication watchdog timer.
class atCommandCOMWDG : public atCommand{
public:
	std::string toString(const unsigned int sequenceNumber);
};

//! Ask the drone to calibrate the magnetometer. Must be flying.
class atCommandCALIB : public atCommand{
public:
	std::string toString(const unsigned int sequenceNumber);
};

#endif