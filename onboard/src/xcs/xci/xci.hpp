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
    /// Unique name in Xci for sensor. 
    std::string name; 
    /// Semantic type of the sensor (ACCELEROMETR, CAMERA etc.). 
    std::string semanticType; 
    /// Fully qualified name of data type (e.g. std::string) 
    std::string syntacticType; 
    /// Additional information about sensor in plain text. Example: "Resolution:1280x720,FPS:20" 
    InformationMap additionalInformation; 

    /// Fill information in variables
    Sensor(const std::string& name, const std::string& semanticType, const std::string& syntacticType, InformationMap additionalInformation = InformationMap()) :
      name(name),
      semanticType(semanticType),
      syntacticType(syntacticType),
      additionalInformation(additionalInformation) {
    };
};




// Some useful typedefs
typedef std::vector<Sensor> SensorList;
typedef std::vector<std::string> SpecialCMDList;
typedef std::string ParameterValueType;
typedef class Xci* (XciFactoryFunction) (DataReceiver &dataReceiver);

//!  Virtual class for unified x-copter interface

//! Every drone have to set XCI_PARAM_FP_PERSISTENCE configuration parameter 
class Xci {
protected:
    DataReceiver& dataReceiver_;
public:

    /// Set dataReceiver reference
    Xci(DataReceiver& dataReceiver) : dataReceiver_(dataReceiver) {
    };

    /// Empty virtual destructor
    virtual ~Xci(){};

    //! Return name of x-copter Xci
    virtual std::string name() = 0;

    //! Return list of all available sensors on x-copter with their specification
    virtual SensorList sensorList() = 0;

    //! Return  value from x-copter's according to the configuration key
    virtual std::string configuration(const std::string &key) = 0;

    //! Return full x-copter's configuration
    virtual InformationMap configuration() = 0;

    //! Return list of x-copter's special commands 
    virtual SpecialCMDList specialCMD() = 0;

    //! Sets on an quadricopter configuration value according to the key.
    virtual void configuration(const std::string &key, const std::string &value) = 0;

    //! Sets on an quadricopter configuration values according to the InformationMap.
    virtual void configuration(const InformationMap &configuration) = 0;

    //! Take command from list of x-copter's special commands and send it to the x-copter
    virtual void command(const std::string &command) = 0;

    //! Take four parameter for fly controls and send it to the quadricopter
    virtual void flyControl(float roll, float pitch, float yaw, float gaz) = 0;

    //! Initializing XCI for use. Function must be idempotent.
    virtual void init() = 0;

    //! Stopping production of any data from sensors (resources may be kept).
    virtual void stop() = 0;
};
}
}

extern "C" {
XCI_LIB_EXPORT xcs::xci::Xci* CreateXci(xcs::xci::DataReceiver &dataReceiver);
}
#endif
