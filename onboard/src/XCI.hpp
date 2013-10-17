#ifndef XCI_H
#define XCI_H

#include <string>
#include <vector>

//! Types of senzor which x-copter can posses
enum SenzorType{
	AKCELEROMETR, /*!< Akcelerometer senzor. */
	CAMERA, /*!< Camera senzor. */
	DISTANCE, /*!< Distance senzor like ultrasonic, laser etc. */  
	GPS_GALILEO, /*!< Senzor of absolute position on Earth. */
	GYRO, /*!< Gyroscope senzor. */
	UNKNOWN /*!< Unknown type of senzor. Use it for new type of senzor which XCS don´t know. */
};

//! Information about senzor
struct Senzor{
	std::string name; /*!< Unique name in XCI for senzor. */
	SenzorType type; /*!< Type of senzor (AKCELEROMETR, CAMERA etc.). */
	std::string additionalInformation; /*!< Additional information about senzor in plain text. Example: "Resolution:1280x720,FPS:20" */
};

// Some usefull typedefs
typedef std::vector<Senzor> senzorList;
typedef std::vector<std::string> specialCMDList;

//! Virtual class for unified x-copter interface
class XCI{
public:
	//! A pure virtual member returning name of x-copter XCI
	virtual std::string getName()=0;
	//! A pure virtual member returning list of available senzor on x-copter
	virtual senzorList getSenzorList()=0;
	//! A pure virtual member taking specifikation of senzor and returning void pointer to data from desired senzor
	virtual void* getSenzorData(Senzor senzor)=0;
	//! A pure virtual member returning x-copter´s configuration
	virtual std::string getConfiguration()=0;
	//! A pure virtual member returning list of x-copter´s special commands 
	virtual specialCMDList getSpecialCMD()=0;
	
	//! A pure virtual member taking new x-copter´s configuration and send this configuration to the x-copter
	virtual void setConfiguration(std::string configuration)=0;
	//! A pure virtual member taking command from list of x-copter´s special commands and send it to the x-copter
	virtual void sendCommand(std::string command)=0;
	//! A pure virtual member taking four fly parameters and send it ot the x-copter
	virtual void sendFlyParam(double roll,double pitch, double yaw, double gaz)=0;
};

#endif