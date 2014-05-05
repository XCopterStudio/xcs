#ifndef XCI_VREP_H
#define XCI_VREP_H

#include <xcs/xci/xci.hpp>

extern "C" {
#include "vrep/extApi.h"
    /*	#include "extApiCustom.h" if you wanna use custom remote API functions! */
}

namespace xcs {
namespace xci {
namespace vrep{

    class XciVrep : public virtual XCI {
    
    public:

       XciVrep(DataReceiver& dataReceiver);

       std::string name() override;

       xcs::xci::SensorList sensorList() override;

       xcs::xci::ParameterValueType parameter(xcs::xci::ParameterNameType name) override;

       std::string configuration(const std::string &key) override;

       xcs::xci::InformationMap configuration() override;

       xcs::xci::SpecialCMDList specialCMD() override;

       void configuration(const std::string &key, const std::string &value) override;

       void configuration(const xcs::xci::InformationMap &configuration) override;

       void command(const std::string &command) override;

       void flyControl(float roll, float pitch, float yaw, float gaz) override;

       void init() override;

       void reset() override;

       void start() override;

       void stop() override;
    };

}}}


#endif