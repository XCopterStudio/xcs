#include "xci_dodo.hpp"

#include <iostream>
#include <chrono>
#include <xcs/xci/data_receiver.hpp>

using namespace std;
using namespace xcs::xci;
using namespace xcs::xci::dodo;
using namespace xcs::nodes;

/*
 * Constants
 */
const std::string XciDodo::NAME_ = "Dodo Test Drone";
uint8_t XciDodo::frames_[XciDodo::VIDEO_HEIGHT_][XciDodo::VIDEO_WIDTH_][XciDodo::VIDEO_COLORS_];

const size_t XciDodo::ALIVE_FREQ_ = 1;
//
const size_t XciDodo::SENSOR_PERIOD_ = 50;

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
    result.push_back(Sensor("alive", "alive"));
    result.push_back(Sensor("video", "video"));
    return result;
}

ParameterValueType XciDodo::parameter(ParameterNameType name) {
    switch (name) {
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
    size_t clock = 0;
    size_t frameNo = 0;
    while (1) {
        if (clock % (1000 / ALIVE_FREQ_) == 0) {
            dataReceiver_.notify("alive", true);
        }
        if (clock % (1000 / VIDEO_FPS_) == 0) {
            renderFrame<VIDEO_NOISE_>(frameNo);
            BitmapType frame;
            frame.height = VIDEO_HEIGHT_;
            frame.width = VIDEO_WIDTH_;
            frame.data = const_cast<uint8_t *> (reinterpret_cast<const uint8_t *> (frames_));

            dataReceiver_.notify("video", frame);
            frameNo++;
        }

        clock += SENSOR_PERIOD_;
        this_thread::sleep_for(chrono::milliseconds(SENSOR_PERIOD_));
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
