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

const size_t XciDodo::ALIVE_FREQ_ = 1;

const size_t XciDodo::SENSOR_PERIOD_ = 20;

const std::string XciDodo::CMD_VIDEO_LOAD_ = "Load";
const std::string XciDodo::CMD_VIDEO_PLAY_ = "Play";
const std::string XciDodo::CMD_VIDEO_PAUSE_ = "Pause";
const std::string XciDodo::CMD_VIDEO_STOP_ = "Stop";

const std::string XciDodo::CONFIG_VIDEO_FILENAME = "video:filename";
const std::string XciDodo::CONFIG_VIDEO_FPS = "video:fps";

const SpecialCMDList XciDodo::specialCommands_({
    XciDodo::CMD_VIDEO_LOAD_,
    XciDodo::CMD_VIDEO_PLAY_,
    XciDodo::CMD_VIDEO_PAUSE_,
    XciDodo::CMD_VIDEO_STOP_
});

/*
 * Implementation
 */
XciDodo::XciDodo(DataReceiver& dataReceiver) :
  XCI(dataReceiver),
  videoFps_(15),
  videoStatus_(VIDEO_UNLOADED) {

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

    switch (videoStatus_) {
        case VIDEO_UNLOADED:
            if (command == CMD_VIDEO_LOAD_) {
                videoPlayer_.init(configuration(CONFIG_VIDEO_FILENAME));
                videoStatus_ = VIDEO_PAUSED;
            } else if (command == CMD_VIDEO_PLAY_ || command == CMD_VIDEO_PAUSE_ || command == CMD_VIDEO_STOP_) {
                //TODO warning
            }
            break;
        default:
            if (command == CMD_VIDEO_LOAD_) {
                //TODO warning
            } else if (command == CMD_VIDEO_PLAY_) {
                videoStatus_ = VIDEO_PLAYING;
            } else if (command == CMD_VIDEO_PAUSE_) {
                videoStatus_ = VIDEO_PAUSED;
            } else if (command == CMD_VIDEO_STOP_) {
                videoStatus_ = VIDEO_PAUSED;
                videoPlayer_.reset();
            }

            break;

    }
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
        if (clock % (1000 / videoFps_) == 0 && videoStatus_ == VIDEO_PLAYING) {
            BitmapType frame = videoPlayer_.getFrame();
            dataReceiver_.notify("video", frame);
            frameNo++;
        }

        clock += SENSOR_PERIOD_;
        this_thread::sleep_for(chrono::milliseconds(SENSOR_PERIOD_));
    }
}

std::string XciDodo::configuration(const std::string & key) {
    return configuration_[key];
}

InformationMap XciDodo::configuration() {
    return configuration_; // we return copy
}

void XciDodo::configuration(const std::string& key, const std::string & value) {
    if (key == CONFIG_VIDEO_FPS) {
        size_t fps = stoi(value);
        if ((1000 / SENSOR_PERIOD_) % fps != 0) {
            cerr << key << " must be divisor of " << (1000 / SENSOR_PERIOD_) << endl; //TODO change to error
        };
        videoFps_ = fps;
    }

    configuration_[key] = value;
}

void XciDodo::configuration(const InformationMap & configuration) {
    configuration_ = configuration;
}

/*
 * NOT IMPLEMENTED (only for linker)
 */
void XciDodo::reset() {

}

SpecialCMDList XciDodo::specialCMD() {
    return specialCommands_;
}

void XciDodo::start() {

}

void XciDodo::stop() {

}

extern "C" {

XCI* CreateXci(DataReceiver& dataReceiver) {
    return new XciDodo(dataReceiver);
}
}
