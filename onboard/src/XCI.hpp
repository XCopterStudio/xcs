#ifndef XCI_H
#define XCI_H

#include <string>
#include <vector>
#include <map>

typedef std::map<std::string,std::string> informationMap;

//! Types of sensor which x-copter can posses
enum SensorType{
	ACCELEROMETR, /*!< Accelerometer sensor. */
	AUDIO, /*!< Audio sensor like microphone. */
	CAMERA, /*!< Camera sensor. */
	DISTANCE_1D, /*!< Distance sensor like ultrasonic, sharp etc. */  
	DISTANCE_2D, /*!< Distance sensor like 2D laser sick TIM3xx etc. */  
	DISTANCE_3D, /*!< Distance sensor like 3D laser Velodyne HDL-64E  etc. */
	ENVIRONMENTAL, /*!< Environmental sensor like wind sensor  etc. */
	FORCE_TOUCH, /*!< Force, touch sensor*/
	GPS_GALILEO, /*!< Sensor of absolute position on Earth. */
	GYRO, /*!< Gyroscope sensor. */
	UNKNOWN /*!< Unknown type of sensor. Use it for new type of sensor which XCS doesn´t know. */
};

//! Information about sensor
struct Sensor{
	std::string name; /*!< Unique name in XCI for sensor. */
	SensorType type; /*!< Type of sensor (ACCELEROMETR, CAMERA etc.). */
	informationMap additionalInformation; /*!< Additional information about sensor in plain text. Example: "Resolution:1280x720,FPS:20" */
};

// Some useful typedefs
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
	virtual void* getSensorData(const Sensor &sensor)=0;

	//! A pure virtual member returning x-copter´s configuration value
	virtual std::string getConfiguration(const std::string &key) = 0;

	//! A pure virtual member returning x-copter´s configuration values
	virtual informationMap getConfiguration() = 0;
	
	//! A pure virtual member returning list of x-copter´s special commands 
	virtual specialCMDList getSpecialCMD()=0;	
	
	//! A pure virtual member sets configuration value.
	virtual int setConfiguration(const std::string &key, const std::string &value) = 0;

	//! A pure virtual member taking new x-copter´s configuration and send this configuration to the x-copter
	virtual int setConfiguration(const informationMap &configuration)=0;
	
	//! A pure virtual member taking command from list of x-copter´s special commands and sending it to the x-copter
	virtual void sendCommand(const std::string &command)=0;
	
	//
	// Bellow are methods important for simple manually driven skeleton.
	//
	
	//! A pure virtual member taking four fly parameters and send it to the x-copter
	virtual void sendFlyParam(double roll, double pitch, double yaw, double gaz)=0;

	//! A pure virtual member initializing XCI for use
	virtual void init()=0;
	
	//! Resets settings to default values and re-calibrates the sensors (if supported).
	virtual void reset() = 0;

	//! Turns on the engines.
	virtual void start() = 0;

	//! Turns off the engines.
	virtual void stop() = 0;
};

#endif
