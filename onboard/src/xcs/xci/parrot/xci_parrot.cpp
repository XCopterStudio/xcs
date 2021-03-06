#include "xci_parrot.hpp"

#include <array>
#include <iostream>
#include <cstring>

#include <xcs/types/bitmap_type.hpp>
#include <xcs/logging.hpp>

#include "video_encapsulation.h"
#include "video_decode.hpp"

#include "configure_options.hpp"

using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace xcs;
using namespace xcs::xci;
using namespace xcs::xci::parrot;
// ----------------- Constant ----------------------- //

const float XciParrot::EPSILON = (float) 1.0e-10;

const std::string XciParrot::NAME = "Parrot AR Drone 2.0 Xci";

// ----------------- Private function --------------- //

void XciParrot::initNetwork() {
    // connect to video port
    atCommandSender_.connect();
    videoReceiver_.connect();
    navdataReceiver_.connect();
    configurationReceiver_.connect();

    threadSendingATCmd_ = std::move(std::thread(boost::bind(&boost::asio::io_service::run, &io_serviceCMD_)));
    threadReadVideoReceiver_ = std::move(std::thread(boost::bind(&boost::asio::io_service::run, &io_serviceVideo_)));
    threadReceiveNavData_ = std::move(std::thread(boost::bind(&boost::asio::io_service::run, &io_serviceNavdata_)));
    threadConfiguration_ = std::move(std::thread(boost::bind(&boost::asio::io_service::run, &io_serviceConfiguration_)));
}

void XciParrot::processVideoData() {
    while (!endAll_) {
        VideoFramePtr frame = nullptr;
        if (videoReceiver_.tryGetVideoFrame(frame)) {
            AVPacket avPacket;
            avPacket.data = &frame->data[frame->payload_offset];
            avPacket.size = frame->payload_size - frame->payload_offset;
            if (videoDecoder_.decodeVideo(&avPacket)) {
                AVFrame* avFrame = videoDecoder_.decodedFrame();
                BitmapType bitmap(avFrame->width, avFrame->height, avFrame->data[0]);
                dataReceiver_.notify("video", bitmap);
                dataReceiver_.notify("internalTimeVideo", frame->timestamp);
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}

bool XciParrot::setConfirmedConfigure(AtCommandPtr command) {

    unsigned int count = 0;
    do {
        atCommandQueue_.push(AtCommandPtr(new AtCommandCONFIG_IDS("0a1b2c3d", "0a1b2c3d", "0a1b2c3d")));
        atCommandQueue_.push(AtCommandPtr(command->clone()));
        this_thread::sleep_for(std::chrono::milliseconds(20));
        if (++count > 20) {
            XCS_LOG_WARN("Cannot receive ack.");
            return false;
        }
    } while (!state_.getState(FLAG_ARDRONE_COMMAND_MASK));

    count = 0;
    do {
        atCommandQueue_.push(AtCommandPtr(new AtCommandCTRL(STATE_ACK_CONTROL_MODE)));
        this_thread::sleep_for(std::chrono::milliseconds(20));

        if (++count >= 20) {
            XCS_LOG_WARN("Cannot receive clear ack.");
            return false;
        }
    } while (state_.getState(FLAG_ARDRONE_COMMAND_MASK));

    return true;
}

bool XciParrot::setDefaultConfiguration() {
    setConfirmedConfigure(AtCommandPtr(new AtCommandCONFIG("custom:application_id", "0a1b2c3d")));
    setConfirmedConfigure(AtCommandPtr(new AtCommandCONFIG("custom:profile_id", "0a1b2c3d")));
    setConfirmedConfigure(AtCommandPtr(new AtCommandCONFIG("custom:session_id", "0a1b2c3d")));

    std::stringstream value;
    //value << H264_720P_CODEC;
    value << H264_360P_CODEC;
    setConfirmedConfigure(AtCommandPtr(new AtCommandCONFIG("video:video_codec", value.str())));
    setConfirmedConfigure(AtCommandPtr(new AtCommandCONFIG("video:bitrate_control_mode", "1")));
    setConfirmedConfigure(AtCommandPtr(new AtCommandCONFIG("video:max_bitrate", "4000")));
    setConfirmedConfigure(AtCommandPtr(new AtCommandCONFIG("video:video_channel", "0")));

    // set which data will be send
    unsigned int ndOptions = ((1 << NAVDATA_DEMO_TAG) |
            (1 << NAVDATA_ALTITUDE_TAG) |
            (1 << NAVDATA_RAW_MEASURES_TAG) |
            (1 << NAVDATA_MAGNETO_TAG) |
            (1 << NAVDATA_TIME_TAG));

    stringstream ndOptionsString;
    ndOptionsString << ndOptions;
    setConfirmedConfigure(AtCommandPtr(new AtCommandCONFIG("general:navdata_options", ndOptionsString.str())));
    //receive only reduced navdata set
    setConfirmedConfigure(AtCommandPtr(new AtCommandCONFIG("general:navdata_demo", "TRUE")));
    // disable visual detection
    setConfirmedConfigure(AtCommandPtr(new AtCommandCONFIG("detect:detect_type", "3")));

    // set max altitude
    setConfirmedConfigure(AtCommandPtr(new AtCommandCONFIG("control:altitude_max", "5000")));

    return true;
}

// ----------------- Public function ---------------- //

XciParrot::XciParrot(DataReceiver &dataReceiver, std::string ipAddress)
  : Xci(dataReceiver),
  inited_(false),
  atCommandSender_(atCommandQueue_, io_serviceCMD_, ipAddress),
  videoReceiver_(io_serviceVideo_, ipAddress),
  navdataReceiver_(dataReceiver, atCommandQueue_, state_, io_serviceNavdata_, ipAddress),
  configurationReceiver_(atCommandQueue_, configuration_, io_serviceConfiguration_, ipAddress) {
    configuration_["XCI_PARAM_FP_PERSISTENCE"] = "50";
};

void XciParrot::init() {
    dataReceiver_.enabled(true);
    if (inited_) {
        return;
    }
    inited_ = true;
    endAll_ = false;

    initNetwork();

    // init videoDecoder
    videoDecoder_.init(AV_CODEC_ID_H264);
    threadReadVideoData_ = std::move(std::thread(&XciParrot::processVideoData, this));

    setDefaultConfiguration();
    configurationReceiver_.update();
}

void XciParrot::stop() {
    dataReceiver_.enabled(false);
}

std::string XciParrot::name() {
    return NAME;
}

SensorList XciParrot::sensorList() {
    SensorList sensorList;

    sensorList.push_back(Sensor("rotation", "ROTATION", "xcs::EulerianVector"));
    sensorList.push_back(Sensor("velocity", "VELOCITY_LOC", "xcs::CartesianVector"));

    sensorList.push_back(Sensor("altitudeAll", "ALTITUDE_ALL", "xcs::CartesianVector"));
    sensorList.push_back(Sensor("altitudeV", "ALTITUDE_V", "double"));
    sensorList.push_back(Sensor("acceleration", "ACCELERATION", "xcs::CartesianVector"));
    sensorList.push_back(Sensor("gyro", "GYRO_RAW", "xcs::CartesianVector"));
    sensorList.push_back(Sensor("magneto", "MAGNETO_RAW", "xcs::CartesianVector"));
    sensorList.push_back(Sensor("internalTimeImu", "TIME_LOC", "double"));

    sensorList.push_back(Sensor("altitude", "ALTITUDE", "double"));
    sensorList.push_back(Sensor("battery", "BATTERY", "double"));
    sensorList.push_back(Sensor("video", "CAMERA", "xcs::BitmapType"));
    sensorList.push_back(Sensor("internalTimeVideo", "TIME_LOC", "double"));

    sensorList.push_back(Sensor("alive", "ALIVE", "int"));

    sensorList.push_back(Sensor("wifiQuality","QUALITY","int"));

    return sensorList;
}

std::string XciParrot::configuration(const std::string &key) {
    if (configuration_.count(key) > 0) {
        return configuration_[key];
    } else {
        return "";
    }
}

InformationMap XciParrot::configuration() {
    return configuration_;
}

SpecialCMDList XciParrot::specialCMD() {
    SpecialCMDList CMDList;
    CMDList.push_back("TakeOff");
    CMDList.push_back("Land");
    CMDList.push_back("EmergencyStop");
    CMDList.push_back("Normal");
    return CMDList;
}

void XciParrot::configuration(const std::string &key, const std::string &value) {
    configuration_[key] = value;
    setConfirmedConfigure(AtCommandPtr(new AtCommandCONFIG(key, value)));
}

void XciParrot::configuration(const InformationMap &configuration) {
    for (auto element : configuration) {
        configuration_[element.first] = element.second;
        setConfirmedConfigure(AtCommandPtr(new AtCommandCONFIG(element.first, element.second)));
    }
}

void XciParrot::command(const std::string &command) {
    if (command == "TakeOff") {
        atCommandQueue_.push(AtCommandPtr(new AtCommandRef(STATE_TAKEOFF)));
    } else if (command == "Land") {
        atCommandQueue_.push(AtCommandPtr(new AtCommandRef(STATE_LAND)));
    } else if (command == "EmergencyStop") {
        if (!state_.getState(FLAG_ARDRONE_EMERGENCY_MASK)) {
            atCommandQueue_.push(AtCommandPtr(new AtCommandRef(STATE_EMERGENCY)));
        }
    } else if (command == "Normal") {
        if (state_.getState(FLAG_ARDRONE_EMERGENCY_MASK)) {
            atCommandQueue_.push(AtCommandPtr(new AtCommandRef(STATE_NORMAL)));
        }
    } else {
        XCS_LOG_WARN("Unsupported command " + command);
    }
}

void XciParrot::flyControl(float roll, float pitch, float yaw, float gaz) {
    if (std::abs(pitch) < EPSILON && std::abs(roll) < EPSILON) {
        atCommandQueue_.push(AtCommandPtr(new AtCommandPCMD(DroneMove(roll, pitch, yaw, gaz))));
    } else {
        atCommandQueue_.push(AtCommandPtr(new AtCommandPCMD(DroneMove(roll, pitch, yaw, gaz), false, false, true)));
    }

}

XciParrot::~XciParrot() {
    endAll_ = true;

    // wait for all thread ends and then clear memory
    io_serviceCMD_.stop();
    if (threadSendingATCmd_.joinable()){
        threadSendingATCmd_.join();
    }

    io_serviceVideo_.stop();
    if (threadReadVideoReceiver_.joinable()){
        threadReadVideoReceiver_.join();
    }

    if (threadReadVideoData_.joinable()){
        threadReadVideoData_.join();
    }

    io_serviceNavdata_.stop();
    if (threadReceiveNavData_.joinable()){
        threadReceiveNavData_.join();
    }

    io_serviceConfiguration_.stop();
    if (threadConfiguration_.joinable()){
        threadConfiguration_.join();
    }
}

extern "C" {

Xci* CreateXci(DataReceiver &dataReceiver) {
    return new XciParrot(dataReceiver);
}
}
