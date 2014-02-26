#include "xci_dodo.hpp"

#include <iostream>
#include <chrono>
#include <xcs/xci/data_receiver.hpp>

using namespace std;
using namespace xcs::xci;
using namespace xcs::xci::dodo;

/*
 * Constants
 */
const std::string XciDodo::NAME_ = "Dodo Test Drone";

/*
 * Implementation
 */
XciDodo::XciDodo(DataReceiver& dataReceiver) : XCI(dataReceiver) {

}

XciDodo::~XciDodo() {

}

void XciDodo::init() {
    inited_ = true;
    sensorThread_ = move(thread(&XciDodo::sensorGenerator, this));
}

std::string XciDodo::name() {
    return NAME_;
}

SensorList XciDodo::sensorList() {
    SensorList result;
    //TODO figure out better initialization of sensor list
    Sensor sensor;
    sensor.name = "alive";
    sensor.semanticType = "alive";
    result.push_back(sensor);
    return result;
}

ParameterValueType XciDodo::parameter(ParameterNameType name) {
    switch(name) {
        case XCI_PARAM_FP_PERSISTENCE:
            return "1000";
        default:
            throw std::runtime_error("Parameter not defined.");
    }
}


void XciDodo::command(const std::string& command) {
    cout << "[dodo] command: " << command << endl;
}

void XciDodo::flyParam(float roll, float pitch, float yaw, float gaz) {
    cout << "[dodo] flyParam: " << roll << ", " << pitch << ", " << yaw << ", " << gaz << endl;
}

void XciDodo::sensorGenerator() {
    while (1) {
        dataReceiver_.notify("alive", true);
        this_thread::sleep_for(chrono::milliseconds(1000));
    }
}

/*
 * NOT IMPLEMENTED (only for linker)
 */
std::string XciDodo::configuration(const std::string& key) {
    return "";
}

InformationMap XciDodo::configuration() {
    return InformationMap();
}

void XciDodo::configuration(const std::string& key, const std::string& value) {

}

void XciDodo::configuration(const InformationMap &configuration) {

}

void XciDodo::reset() {

}

SpecialCMDList XciDodo::specialCMD() {
    return SpecialCMDList();
}

void XciDodo::start() {

}

void XciDodo::stop() {

}

extern "C" {
    XCI* CreateXci(DataReceiver &dataReceiver) { 
        return new XciDodo(dataReceiver); 
    }
}
