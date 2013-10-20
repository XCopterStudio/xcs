#ifndef XCI_H
#define XCI_H

#include <string>
#include <vector>

//! Types of sensor which x-copter can posses
enum SensorType{
	ACCELEROMETR, /*!< Accelerometer sensor. */
	CAMERA, /*!< Camera sensor. */
	DISTANCE, /*!< Distance sensor like ultrasonic, laser etc. */  
	GPS_GALILEO, /*!< Sensor of absolute position on Earth. */
	GYRO, /*!< Gyroscope sensor. */
	UNKNOWN /*!< Unknown type of sensor. Use it for new type of sensor which XCS doesn´t know. */
};

//! Information about sensor
struct Sensor{
	std::string name; /*!< Unique name in XCI for sensor. */
	SensorType type; /*!< Type of sensor (ACCELEROMETR, CAMERA etc.). */
	std::string additionalInformation; /*!< Additional information about sensor in plain text. Example: "Resolution:1280x720,FPS:20" */
};

// Some usefull typedefs
typedef std::vector<Sensor> sensorList;
typedef std::vector<std::string> specialCMDList;

//! Virtual class for unified x-copter interface
class XCI{
public:
	//
	// Bellow are methods that are subject to change based on the discussions and real-world needs.
	//
	
	//! A pure virtual member returning name of x-copter XCI
	virtual std::string getName()=0;

	//! A pure virtual member returning list of available sensors on x-copter
	virtual sensorList getSensorList()=0;

	//! A pure virtual member taking specification of sensor and returning void pointer to data from desired sensor
	virtual void* getSensorData(Sensor sensor)=0;

	//! Returning x-copter´s configuration value
	virtual std::string getConfiguration(const std::string &key) = 0;
	
	//! Sets configuration value.
	virtual setConfiguration(const std::string &key, const std::string &value) = 0;
	
	//! A pure virtual member returning list of x-copter´s special commands 
	virtual specialCMDList getSpecialCMD()=0;	
	
	//! A pure virtual member taking new x-copter´s configuration and send this configuration to the x-copter
	virtual void setConfiguration(std::string configuration)=0;
	
	//! A pure virtual member taking command from list of x-copter´s special commands and sending it to the x-copter
	virtual void sendCommand(std::string command)=0;
	
	//
	// Bellow are methods important for simple manually driven skeleton.
	//
	
	//! A pure virtual member taking four fly parameters and send it ot the x-copter
	virtual void sendFlyParam(double roll,double pitch, double yaw, double gaz)=0;

	//! Resets settings to default values and re-calibrates the sensors (if supported).
	virtual void reset() = 0;

	//! Turns on the engines.
	virtual void start() = 0;

	//! Turns off the engines.
	virtual void stop() = 0;
};

#endif
