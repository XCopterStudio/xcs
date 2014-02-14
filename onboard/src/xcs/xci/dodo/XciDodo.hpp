#ifndef XCI_DODO_H
#define XCI_DODO_H

#include "xcs/xci/XCI.hpp"

#include <thread>

namespace xcs {
namespace xci {
namespace dodo {

class XciDodo : public virtual XCI {
public:
    XciDodo(DataReceiver& dataReceiver);
    ~XciDodo();

    virtual std::string name();

    //! A pure virtual member returning list of available sensors on x-copter
    virtual SensorList sensorList();

    //! A pure virtual member returning x-copter's configuration value
    virtual std::string configuration(const std::string &key);

    //! A pure virtual member returning x-copter's configuration values
    virtual InformationMap configuration();

    //! A pure virtual member returning list of x-copter's special commands 
    virtual SpecialCMDList specialCMD();

    //! A pure virtual member sets configuration value.
    virtual void configuration(const std::string &key, const std::string &value);

    //! A pure virtual member taking new x-copter's configuration and send this configuration to the x-copter
    virtual void configuration(const InformationMap &configuration);

    //! A pure virtual member taking command from list of x-copter's special commands and sending it to the x-copter
    virtual void command(const std::string &command);

    //! A pure virtual member taking four fly parameters and send it to the x-copter
    virtual void flyParam(float roll, float pitch, float yaw, float gaz);

    //! A pure virtual member initializing XCI for use
    virtual void init();

    //! Resets settings to default values and re-calibrates the sensors (if supported).
    virtual void reset();

    //! Turns on the engines.
    virtual void start();

    //! Turns off the engines.
    virtual void stop();
private:
    static const std::string NAME_;
    bool inited_;
    std::thread sensorThread_;
    
    //! Generates dummy data for sensors
    void sensorGenerator();
};
}
}
}
#endif