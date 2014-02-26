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
uint8_t XciDodo::frames_[XciDodo::VIDEO_LENGTH_][XciDodo::VIDEO_WIDTH_][XciDodo::VIDEO_HEIGHT_][3];

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
    renderFrames();
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
            BitmapType frame;
            frame.height = VIDEO_HEIGHT_;
            frame.width = VIDEO_WIDTH_;
            frame.data = const_cast<uint8_t *> (reinterpret_cast<const uint8_t *> (frames_ + frameNo));

            dataReceiver_.notify("video", frame);
            frameNo = (frameNo++) % VIDEO_LENGTH_;
        }

        clock += SENSOR_PERIOD_;
        this_thread::sleep_for(chrono::milliseconds(SENSOR_PERIOD_));
    }
}

void XciDodo::renderFrames() {
    static const size_t width = 3;
    static const size_t speed = 10;

    for (auto frame = 0; frame < VIDEO_LENGTH_; ++frame) {
        for (auto x = 0; x < VIDEO_WIDTH_; ++x) {
            auto linepos = VIDEO_WIDTH_ / 2 + frame * speed;
            for (auto y = 0; y < VIDEO_WIDTH_; ++y) {
                auto color = (x >= linepos && x < linepos + width) ? 0 : 255;
                frames_[frame][x][y][0] = color;
                frames_[frame][x][y][1] = color;
                frames_[frame][x][y][2] = color;
            }
        }
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
