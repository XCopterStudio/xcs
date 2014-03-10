#include "xci_dodo.hpp"

#include <iostream>
#include <chrono>
#include <string>
#include <boost/log/trivial.hpp>

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
  videoFps_(10),
  videoStatus_(VIDEO_UNLOADED) {

}

XciDodo::~XciDodo() {

}

void XciDodo::init() {
    inited_ = true;
    sensorThread_ = move(thread(&XciDodo::sensorGenerator, this));
    configuration(CONFIG_VIDEO_FPS, to_string(videoFps_)); // back-propagation of default value
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
    BOOST_LOG_TRIVIAL(info) << "[dodo] command: " << command;

    switch (videoStatus_) {
        case VIDEO_UNLOADED:
            if (command == CMD_VIDEO_LOAD_) {
                videoPlayer_.init(configuration(CONFIG_VIDEO_FILENAME));
                videoStatus_ = VIDEO_PAUSED;
            } else if (command == CMD_VIDEO_PLAY_ || command == CMD_VIDEO_PAUSE_ || command == CMD_VIDEO_STOP_) {
                throw runtime_error("Cannot execute command in unloaded state.");
            }
            break;
        default:
            if (command == CMD_VIDEO_LOAD_) {
                throw runtime_error("Cannot execute command in loaded state.");
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
    BOOST_LOG_TRIVIAL(info) << "[dodo] flyParam: " << roll << ", " << pitch << ", " << yaw << ", " << gaz;
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
            BOOST_LOG_TRIVIAL(error) << key << " must be divisor of " << (1000 / SENSOR_PERIOD_);
            throw runtime_error("FPS must be proper divisor."); // stupid urbi... must print error and throw exception as well
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
