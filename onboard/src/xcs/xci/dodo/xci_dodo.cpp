#include "xci_dodo.hpp"

#include <iostream>
#include <chrono>
#include <string>

#include <xcs/xcs_fce.hpp>
#include <xcs/logging.hpp>
#include <xcs/xci/data_receiver.hpp>

using namespace std;
using namespace xcs;
using namespace xcs::xci;
using namespace xcs::xci::dodo;

/*
 * Constants
 */
const std::string XciDodo::NAME_ = "Dodo Test Drone";

const size_t XciDodo::ALIVE_FREQ_ = 1;
const size_t XciDodo::NAVDATA_FREQ_ = 20;

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
const std::string XciDodo::CONFIG_LOG_FC = "log:fc";
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
  Xci(dataReceiver),
  inited_(false),
  runAll_(true),
  videoFps_(0),
  videoStatus_(VIDEO_UNLOADED) {

    configuration_["XCI_PARAM_FP_PERSISTENCE"] = "50";
}

XciDodo::~XciDodo() {
    runAll_ = false;

    // wait for all thread ends and then clear memory
    if (sensorThread_.joinable()) {
        sensorThread_.join();
    }

    if (videoThread_.joinable()) {
        videoThread_.join();
    }
}

void XciDodo::init() {
    dataReceiver_.enabled(true);
    if (inited_) {
        return;
    }
    inited_ = true;

    // back-propagation of default values
    configuration(CONFIG_VIDEO_FPS, to_string(videoFps_));
    configuration(CONFIG_VIDEO_FONT, "/usr/share/fonts/truetype/liberation/LiberationMono-Bold.ttf");
    configuration(CONFIG_VIDEO_TIMESTAMPS, "0");
    configuration(CONFIG_LOG_FC, "0");
    configuration(CONFIG_LOG_COMMAND, "1");

    // start threads
    sensorThread_ = move(thread(&XciDodo::sensorGenerator, this));
    videoThread_ = move(thread(&XciDodo::videoPlayer, this));

}

void XciDodo::stop() {
    videoStatus_ = VIDEO_PAUSED;
    dataReceiver_.enabled(false);
}

std::string XciDodo::name() {
    return NAME_;
}

SensorList XciDodo::sensorList() {
    SensorList result;
    result.push_back(Sensor("alive", "ALIVE", "int"));
    result.push_back(Sensor("video", "CAMERA", "xcs::BitmapType"));
    result.push_back(Sensor("altitude", "ALTITUDE", "double"));
    result.push_back(Sensor("rotation", "ROTATION", "xcs::EulerianVector"));
    result.push_back(Sensor("velocity", "VELOCITY_LOC", "xcs::CartesianVector"));
    return result;
}

void XciDodo::command(const std::string& command) {
    if (stoi(configuration(CONFIG_LOG_COMMAND))) {
        XCS_LOG_INFO("[dodo] command: " << command);
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

void XciDodo::flyControl(float roll, float pitch, float yaw, float gaz) {
    if (stoi(configuration(CONFIG_LOG_FC))) {
        XCS_LOG_INFO("[dodo] flyControl: " << roll << ", " << pitch << ", " << yaw << ", " << gaz);
    }

    velocity_.x = valueInRange<double>(roll, 1.0) * 0.05; // very simple
    velocity_.y = -valueInRange<double>(pitch, 1.0) * 0.05; // very simple
    velocity_.z = valueInRange<double>(gaz, 1.0) * 0.05; // very simple

    position_.x += velocity_.x;
    position_.y += velocity_.y;
    position_.z += velocity_.z;

    rotation_.psi += valueInRange<double>(yaw, 1.0) * 0.05; // very simple
    rotation_.phi = valueInRange<double>(roll, 1.0); // very simple
    rotation_.theta = valueInRange<double>(pitch, 1.0); // very simple
}

void XciDodo::sensorGenerator() {
    size_t clock = 0;
    while (runAll_) {
        if (clock % (1000 / ALIVE_FREQ_) == 0) {
            dataReceiver_.notify("alive", true);
        }
        if (clock % (1000 / NAVDATA_FREQ_) == 0) {
            dataReceiver_.notify("altitude", position_.z);
            dataReceiver_.notify("rotation", rotation_);
            dataReceiver_.notify("velocity", velocity_);
        }

        clock += SENSOR_PERIOD_;
        this_thread::sleep_for(chrono::milliseconds(SENSOR_PERIOD_));
    }
}

void XciDodo::videoPlayer() {
    while (runAll_) {
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
    for (auto element : configuration) {
        configuration_[element.first] = element.second;
    }
}

SpecialCMDList XciDodo::specialCMD() {
    return specialCommands_;
}

extern "C" {

Xci* CreateXci(DataReceiver& dataReceiver) {
    return new XciDodo(dataReceiver);
}
}
