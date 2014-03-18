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
const size_t XciDodo::ALTITUDE_FREQ_ = 100;

const size_t XciDodo::SENSOR_PERIOD_ = 10;
const size_t XciDodo::VIDEO_IDLE_SLEEP_ = 200;

const std::string XciDodo::CMD_VIDEO_LOAD_ = "Load";
const std::string XciDodo::CMD_VIDEO_PLAY_ = "Play";
const std::string XciDodo::CMD_VIDEO_PAUSE_ = "Pause";
const std::string XciDodo::CMD_VIDEO_STOP_ = "Stop";
const std::string XciDodo::CMD_VIDEO_STEP_ = "Step";

const std::string XciDodo::CONFIG_VIDEO_FILENAME = "video:filename";
const std::string XciDodo::CONFIG_VIDEO_FPS = "video:fps";
const std::string XciDodo::CONFIG_VIDEO_FONT = "video:font";
const std::string XciDodo::CONFIG_VIDEO_TIMESTAMPS = "video:timestamps";
const std::string XciDodo::CONFIG_LOG_FP = "log:fp";
const std::string XciDodo::CONFIG_LOG_COMMAND = "log:command";

const SpecialCMDList XciDodo::specialCommands_({
    XciDodo::CMD_VIDEO_LOAD_,
    XciDodo::CMD_VIDEO_PLAY_,
    XciDodo::CMD_VIDEO_PAUSE_,
    XciDodo::CMD_VIDEO_STOP_,
    XciDodo::CMD_VIDEO_STEP_
});

/*
 * Implementation
 */
XciDodo::XciDodo(DataReceiver& dataReceiver) :
  XCI(dataReceiver),
  inited_(false),
  videoFps_(0),
  videoStatus_(VIDEO_UNLOADED) {

}

XciDodo::~XciDodo() {

}

void XciDodo::init() {
    if (inited_) {
        return;
    }
    inited_ = true;
    // reset sensors (pseudophysical state)
    altitude_ = 0;

    // back-propagation of default values
    configuration(CONFIG_VIDEO_FPS, to_string(videoFps_));
    configuration(CONFIG_VIDEO_FONT, "/usr/share/fonts/truetype/liberation/LiberationMono-Bold.ttf");
    configuration(CONFIG_VIDEO_TIMESTAMPS, "0");
    configuration(CONFIG_LOG_FP, "0");
    configuration(CONFIG_LOG_COMMAND, "1");

    // start threads
    sensorThread_ = move(thread(&XciDodo::sensorGenerator, this));
    videoThread_ = move(thread(&XciDodo::videoPlayer, this));

}

std::string XciDodo::name() {
    return NAME_;
}

SensorList XciDodo::sensorList() {
    SensorList result;
    result.push_back(Sensor("alive", "alive"));
    result.push_back(Sensor("video", "video"));
    result.push_back(Sensor("altitude", "altitude"));
    return result;
}

ParameterValueType XciDodo::parameter(ParameterNameType name) {
    switch (name) {
        case XCI_PARAM_FP_PERSISTENCE:
            return "50";
        default:
            throw std::runtime_error("Parameter not defined.");
    }
}

void XciDodo::command(const std::string& command) {
    if (stoi(configuration(CONFIG_LOG_COMMAND))) {
        BOOST_LOG_TRIVIAL(info) << "[dodo] command: " << command;
    }

    switch (videoStatus_) {
        case VIDEO_UNLOADED:
            if (command == CMD_VIDEO_LOAD_) {
                videoPlayer_.init(configuration(CONFIG_VIDEO_FILENAME), configuration(CONFIG_VIDEO_FONT));
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
            } else if (command == CMD_VIDEO_STEP_) {
                videoStatus_ = VIDEO_PAUSED;
                renderFrame();
            } else if (command == CMD_VIDEO_PAUSE_) {
                videoStatus_ = VIDEO_PAUSED;
            } else if (command == CMD_VIDEO_STOP_) {
                videoStatus_ = VIDEO_PAUSED;
                videoPlayer_.rewind();
            }

            break;

    }
}

void XciDodo::flyParam(float roll, float pitch, float yaw, float gaz) {
    if (stoi(configuration(CONFIG_LOG_FP))) {
        BOOST_LOG_TRIVIAL(info) << "[dodo] flyParam: " << roll << ", " << pitch << ", " << yaw << ", " << gaz;
    }

    altitude_ += valueInRange<double>(gaz, 1.0) * 0.05; // very simple
}

void XciDodo::sensorGenerator() {
    size_t clock = 0;
    while (1) {
        if (clock % (1000 / ALIVE_FREQ_) == 0) {
            dataReceiver_.notify("alive", true);
        }
        if (clock % (1000 / ALTITUDE_FREQ_) == 0) {
            cerr << "Alti: " << altitude_ << endl;
            dataReceiver_.notify("altitude", altitude_);
        }

        clock += SENSOR_PERIOD_;
        this_thread::sleep_for(chrono::milliseconds(SENSOR_PERIOD_));
    }
}

void XciDodo::videoPlayer() {
    while (1) {
        if (videoStatus_ == VIDEO_PLAYING) {
            renderFrame();
            auto fps = stoi(configuration(CONFIG_VIDEO_FPS));
            auto sleep = (fps == 0) ? videoPlayer_.framePeriod() : (1000 / fps);
            this_thread::sleep_for(chrono::milliseconds(sleep));
        } else {
            this_thread::sleep_for(chrono::milliseconds(VIDEO_IDLE_SLEEP_));
        }
    }
}

void XciDodo::renderFrame() {
    BitmapType frame = videoPlayer_.getFrame();
    dataReceiver_.notify("video", frame);
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
        videoFps_ = fps;
    }
    if (key == CONFIG_VIDEO_TIMESTAMPS) {
        videoPlayer_.timestamps(stoi(value));
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
