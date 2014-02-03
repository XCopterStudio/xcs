#ifndef XCI_H
#define XCI_H

#include <string>
#include <vector>
#include <map>

#include "data_receiver.hpp"

#if defined (_WIN32) 
    #if defined(xci_EXPORTS)
        #define  XCI_LIB_EXPORT __declspec(dllexport)
    #else
        #define  XCI_LIB_EXPORT __declspec(dllimport)
    #endif /* xci_EXPORTS */
#else /* defined (_WIN32) */
    #define XCI_LIB_EXPORT
#endif

namespace xcs{
namespace xci{

    //! Types of sensor which x-copter can posses

    enum SensorType {
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
        UNKNOWN /*!< Unknown type of sensor. Use it for new type of sensor which XCS doesn�t know. */
    };

    typedef std::map<std::string, std::string> InformationMap;

    //! Information about sensor
    struct Sensor {
        std::string name; /*!< Unique name in XCI for sensor. */
        SensorType type; /*!< Type of sensor (ACCELEROMETR, CAMERA etc.). */
        InformationMap additionalInformation; /*!< Additional information about sensor in plain text. Example: "Resolution:1280x720,FPS:20" */
    };

    // Some useful typedefs
    typedef std::vector<Sensor> SensorList;
    typedef std::vector<std::string> SpecialCMDList;
    typedef class XCI* (XciFactoryFunction)();

    //! Virtual class for unified x-copter interface

    class XCI {
    protected:
        const DataReceiver dataSender_;
    public:
        //
        // Bellow are methods that are subject to change based on the discussions and real-world needs.
        //
        XCI(const DataReceiver dataSender) : dataSender_(dataSender) {};
        //! A pure virtual member returning name of x-copter XCI
        virtual std::string name() = 0;

        //! A pure virtual member returning list of available sensors on x-copter
        virtual SensorList sensorList() = 0;

        //! A pure virtual member returning x-copter�s configuration value
        virtual std::string configuration(const std::string &key) = 0;

        //! A pure virtual member returning x-copter�s configuration values
        virtual InformationMap configuration() = 0;

        //! A pure virtual member returning list of x-copter�s special commands 
        virtual SpecialCMDList specialCMD() = 0;

        //! A pure virtual member sets configuration value.
        virtual void configuration(const std::string &key, const std::string &value) = 0;

        //! A pure virtual member taking new x-copter�s configuration and send this configuration to the x-copter
        virtual void configuration(const InformationMap &configuration) = 0;

        //! A pure virtual member taking command from list of x-copter�s special commands and sending it to the x-copter
        virtual void command(const std::string &command) = 0;

        //
        // Bellow are methods important for simple manually driven skeleton.
        //

        //! A pure virtual member taking four fly parameters and send it to the x-copter
        virtual void flyParam(float roll, float pitch, float yaw, float gaz) = 0;

        //! A pure virtual member initializing XCI for use
        virtual void init() = 0;

        //! Resets settings to default values and re-calibrates the sensors (if supported).
        virtual void reset() = 0;

        //! Turns on the engines.
        virtual void start() = 0;

        //! Turns off the engines.
        virtual void stop() = 0;
    };
}
}

extern "C" {
    XCI_LIB_EXPORT xcs::xci::XCI* CreateXci();
}
#endif
