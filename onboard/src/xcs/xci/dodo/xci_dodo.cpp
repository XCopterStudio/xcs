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
            renderFrame(frameNo, VIDEO_NOISE_);
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

void XciDodo::renderFrame(size_t frameNo, int16_t noise) {
    static const size_t width = 10;
    static const size_t amplitude = 80;
    static const size_t dash = VIDEO_HEIGHT_ / 8;
    static const double skewAmplitude = 0.2;
    static const double speed = 0.1;
    static const double skewSpeed = 0.1;

    auto lineMiddle = (VIDEO_WIDTH_ / 2) + static_cast<size_t> (amplitude * sin(frameNo * speed));
    auto skew = skewAmplitude * sin(frameNo * skewSpeed);
    for (auto y = 0; y < VIDEO_HEIGHT_; ++y) {
        size_t seed = rand();
        auto linePos = lineMiddle + skew * y;
        for (auto x = 0; x < VIDEO_WIDTH_; ++x) {
            int16_t color = (((y / dash) % 2 == 0) && x >= linePos && x < linePos + width) ? 0 : 255;
            if (noise > 0) {
                //color += rand() % (2 * noise) - noise;
                // calling rand for each pixel is surprisingly CPU expensive, so we use this hand-made low-entropy random number generator
                color += ((x * frameNo * seed + x * lineMiddle * y + frameNo) % 7919) % (2 * noise) - noise;
            }
            frames_[y][x][0] = static_cast<uint8_t> (valueInRange<int16_t>(color, 0, 255));
            frames_[y][x][1] = static_cast<uint8_t> (valueInRange<int16_t>(color, 0, 255));
            frames_[y][x][2] = static_cast<uint8_t> (valueInRange<int16_t>(color, 0, 255));
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
