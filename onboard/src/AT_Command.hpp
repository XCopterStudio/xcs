#ifndef AT_COMMAND_H
#define AT_COMMAND_H

#include <string>
#include <cstdint>

//! Basic behavior of parrot drone
enum behavior{
	TAKEOFF,
	LAND,
	EMERGENCY,
	NORMAL,
	CLEAR
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
//! correct input value to this range. All values lower or higher than limits
//! will be replaced with this limits. 
const struct droneMove{
	float_int roll; /*< */
	float_int pitch;/*< */
	float_int yaw;	/*< */
	float_int gaz;	/*< */

	droneMove(float roll = 0, float pitch = 0, float yaw = 0, float gaz = 0);
};

//! Basic class representing at command from which all at commands must be inherited.

//! At command class serve only as general pointer to the inherited commands and printing.
class atCommand{
public:
	//! Return text representation of at command.
	/*!
		\param sequenceNumber have to be in increasing order. Drone discard all at command with lower seqNumber than last one.
		\return text representation of an at command according to ar drone 2.0. documentation
	*/
	virtual std::string toString(const unsigned int sequenceNumber) = 0;
};

//! Drone take off, landing and emergency stop command.

//! When you want change drone state from emergency to normal or otherwise
//! first check in which state drone is because both commands have same 
//! syntax. And when you send more then one command for changing drone state. 
//! Drone will flip flop between this states.
const class atCommandRef : public atCommand{
	static const std::string nameOfCommand; /*!< AT command name according to the ar drone 2.0 documentation. */
	behavior basicBehavior; /*< Represent behavior of drone (take off, land etc.).*/
public:
	//! Initialize constant class representing at command for drone movement.
	atCommandRef(const behavior basicBehavior) : basicBehavior(basicBehavior) {};
	std::string toString(const unsigned int sequenceNumber);
};

//! Move drone command.
const class atCommandPCMD : public atCommand{
	static const std::string nameOfCommand; /*!< AT command name according to the ar drone 2.0 documentation. */
	droneMove move;
	bool absoluteControl;
	bool combinedYaw;
	bool progressivCommand;
public:
	//! Initialize constant class representing at command for drone movement.
	/*!
	\param movement is state which drone will try accomplish.
	\param absControl set whether drone will use absolute control mode or not. Absolute control mode switch between controller frame and ar drone frame more in documentation.
	\param combYaw enable combined yaw control for generating complete turns (phi+yaw) from phi parameter.
	*/
	atCommandPCMD(const droneMove movement,const bool absControl = false, const bool combYaw = false,const bool progCmd = true);
	std::string toString(const unsigned int sequenceNumber);
};

//! Move drone command with absolute control support. 
const class atCommandPCMD_MAG : public atCommandPCMD{
	static const std::string nameOfCommand; /*!< AT command name according to the ar drone 2.0 documentation. */
	float_int magnetoAngle;
	float_int magnetoAccuracy;
public:
	atCommandPCMD_MAG(const droneMove movement,const float psi, const float psiAccur, const bool absControl = false, const bool combYaw = false,const bool progCmd = true);
	std::string toString(const unsigned int sequenceNumber);
};

//! Sets the reference for horizontal plane. Must be on the ground.
const class atCommandFTRIM : public atCommand{
	static const std::string nameOfCommand; /*!< AT command name according to the ar drone 2.0 documentation. */
public:
	std::string toString(const unsigned int sequenceNumber);
};

//! Configuration of the drone.
const class atCommandCONFIG : public atCommand{
	static const std::string nameOfCommand; /*!< AT command name according to the ar drone 2.0 documentation. */
public:
	std::string toString(const unsigned int sequenceNumber);
};

//! Identifiers for atCommandConfig.
const class atCommandCONFIG_IDS : public atCommand{
	static const std::string nameOfCommand; /*!< AT command name according to the ar drone 2.0 documentation. */
public:
	std::string toString(const unsigned int sequenceNumber);
};

//! Reset the communication watchdog timer.
const class atCommandCOMWDG : public atCommand{
	static const std::string nameOfCommand; /*!< AT command name according to the ar drone 2.0 documentation. */
public:
	std::string toString(const unsigned int sequenceNumber);
};

//! Ask the drone to calibrate the magnetometer. Must be flying.
const class atCommandCALIB : public atCommand{
	static const std::string nameOfCommand; /*!< AT command name according to the ar drone 2.0 documentation. */
public:
	std::string toString(const unsigned int sequenceNumber);
};

#endif