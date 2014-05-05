#include "xci_vrep.hpp"

using namespace xcs::xci;
using namespace xcs::xci::vrep;

XciVrep::XciVrep(DataReceiver& dataReceiver) : XCI(dataReceiver)
{
}

std::string XciVrep::name(){
    return "xci_vrep";
}

SensorList XciVrep::sensorList(){
    return SensorList();
}

ParameterValueType XciVrep::parameter(ParameterNameType name){
    return ParameterValueType();
}

std::string XciVrep::configuration(const std::string &key){
    return "";
}

InformationMap XciVrep::configuration(){
    return InformationMap();
}

SpecialCMDList XciVrep::specialCMD(){
    return SpecialCMDList();
}

void XciVrep::configuration(const std::string &key, const std::string &value){}

void XciVrep::configuration(const InformationMap &configuration){}

void XciVrep::command(const std::string &command){}

void XciVrep::flyControl(float roll, float pitch, float yaw, float gaz){}

void XciVrep::init(){}

void XciVrep::reset(){}

void XciVrep::start(){}

void XciVrep::stop(){}