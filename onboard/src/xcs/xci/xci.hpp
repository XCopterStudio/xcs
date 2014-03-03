/*!
 * \todo Const methods
 */
#ifndef XCI_H
#define XCI_H

#include <string>
#include <vector>
#include <map>

#if defined (_WIN32) 
#if defined(xci_EXPORTS)
#define  XCI_LIB_EXPORT __declspec(dllexport)
#else
#define  XCI_LIB_EXPORT __declspec(dllimport)
#endif /* xci_EXPORTS */
#else /* defined (_WIN32) */
#define XCI_LIB_EXPORT
#endif

namespace xcs {
namespace xci {

typedef std::map<std::string, std::string> InformationMap;

/*!
 * Forward declaration as DataReceiver transitively includes
 * uobject.hh and we don't want to soil XCI.hpp's include path with Urbi's.
 */
class DataReceiver;

//! Information about sensor

struct Sensor {
    std::string name; /*!< Unique name in XCI for sensor. */
    std::string semanticType; /*!< Type of sensor (ACCELEROMETR, CAMERA etc.). */
    InformationMap additionalInformation; /*!< Additional information about sensor in plain text. Example: "Resolution:1280x720,FPS:20" */

    Sensor(std::string name = "", std::string semanticType = "", InformationMap additionalInformation = InformationMap()) : name(name), semanticType(semanticType), additionalInformation(additionalInformation) {
    };
};

enum ParameterNameType {
    //! How long is non-zero fly param active before it's zeroed (in milliseconds).
    XCI_PARAM_FP_PERSISTENCE
};

// Some useful typedefs
typedef std::vector<Sensor> SensorList;
typedef std::vector<std::string> SpecialCMDList;
typedef std::string ParameterValueType;
typedef class XCI* (XciFactoryFunction) (DataReceiver &dataReceiver);

//! Virtual class for unified x-copter interface

class XCI {
protected:
    DataReceiver& dataReceiver_;
public:
    //
    // Bellow are methods that are subject to change based on the discussions and real-world needs.
    //

    XCI(DataReceiver& dataReceiver) : dataReceiver_(dataReceiver) {
    };
    //! A pure virtual member returning name of x-copter XCI
    virtual std::string name() = 0;

    //! A pure virtual member returning list of available sensors on x-copter
    virtual SensorList sensorList() = 0;

    //! A pure virtual member returning specific x-copter's parameter.
    virtual ParameterValueType parameter(ParameterNameType name) = 0;

    //! A pure virtual member returning x-copter's configuration value
    virtual std::string configuration(const std::string &key) = 0;

    //! A pure virtual member returning x-copter's configuration values
    virtual InformationMap configuration() = 0;

    //! A pure virtual member returning list of x-copter's special commands 
    virtual SpecialCMDList specialCMD() = 0;

    //! A pure virtual member sets configuration value.
    virtual void configuration(const std::string &key, const std::string &value) = 0;

    //! A pure virtual member taking new x-copter's configuration and send this configuration to the x-copter
    virtual void configuration(const InformationMap &configuration) = 0;

    //! A pure virtual member taking command from list of x-copter's special commands and sending it to the x-copter
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
XCI_LIB_EXPORT xcs::xci::XCI* CreateXci(xcs::xci::DataReceiver &dataReceiver);
}
#endif