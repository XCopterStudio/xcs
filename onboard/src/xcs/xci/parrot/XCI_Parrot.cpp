#include "xci_parrot.hpp"

#include <array>
#include <iostream>
#include <cstring>

#include <xcs/types/bitmap_type.hpp>
#include <xcs/logging.hpp>

#include "video_encapsulation.h"
#include "video_decode.hpp"

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

    threadSendingATCmd_ = std::move(std::thread(boost::bind(&boost::asio::io_service::run, &io_serviceCMD_)));
	threadReadVideoReceiver_ = std::move(std::thread(boost::bind(&boost::asio::io_service::run, &io_serviceVideo_)));
	threadReceiveNavData_ = std::move(std::thread(boost::bind(&boost::asio::io_service::run, &io_serviceNavdata_)));
}

void XciParrot::processVideoData(){
    while (!endAll_){
        VideoFramePtr frame = nullptr;
        if (videoReceiver_.tryGetVideoFrame(frame)){
            AVPacket avPacket;
            //cerr << "Video frame [" << frame->width << "," << frame->height << "]" << endl;
            avPacket.data = &frame->data[frame->payload_offset];
            avPacket.size = frame->payload_size - frame->payload_offset;
            if (videoDecoder_.decodeVideo(&avPacket)){
                AVFrame* avFrame = videoDecoder_.decodedFrame();
                //cerr << "Video avframe [" << avFrame->width << "," << avFrame->height << "]" << endl;
                BitmapType bitmap(avFrame->width, avFrame->height, avFrame->data[0]);
                dataReceiver_.notify("video",bitmap);
                dataReceiver_.notify("internalTimeVideo", frame->timestamp);
            }
            //delete frame;
        }
        else{
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}

bool XciParrot::setConfirmedConfigure(AtCommand *command){

    unsigned int count = 0;
    do{
        atCommandQueue_.push(new AtCommandCONFIG_IDS("0a1b2c3d", "0a1b2c3d", "0a1b2c3d"));
        atCommandQueue_.push(command->clone());
        this_thread::sleep_for(std::chrono::milliseconds(10));
        if (++count > 20){
            XCS_LOG_WARN("Cannot receive ack.");
            delete command;
            return false;
        }
    } while (!state_.getState(FLAG_ARDRONE_COMMAND_MASK));
    delete command;

    count = 0;
    do{
        atCommandQueue_.push(new AtCommandCTRL(STATE_ACK_CONTROL_MODE));
        this_thread::sleep_for(std::chrono::milliseconds(10));

        if (++count >= 20){
            XCS_LOG_WARN("Cannot receive clear ack.");
            return false;
        }
    } while (state_.getState(FLAG_ARDRONE_COMMAND_MASK));

    return true;
}

bool XciParrot::setDefaultConfiguration(){
    setConfirmedConfigure(new AtCommandCONFIG("custom:application_id","0a1b2c3d"));
    setConfirmedConfigure(new AtCommandCONFIG("custom:profile_id", "0a1b2c3d"));
    setConfirmedConfigure(new AtCommandCONFIG("custom:session_id", "0a1b2c3d"));

    setConfirmedConfigure(new AtCommandCONFIG("video:video_codec", "129"));
    setConfirmedConfigure(new AtCommandCONFIG("video:bitrate_control_mode", "1"));
    setConfirmedConfigure(new AtCommandCONFIG("video:max_bitrate", "4000"));
    setConfirmedConfigure(new AtCommandCONFIG("video:video_channel", "0"));

    //receive only reduced navdata set
    setConfirmedConfigure(new AtCommandCONFIG("general:navdata_demo", "TRUE"));
    // set which data will be send
    unsigned int ndOptions = ((1 << NAVDATA_DEMO_TAG) |
        (1 << NAVDATA_ALTITUDE_TAG) |
        (1 << NAVDATA_RAW_MEASURES_TAG) |
        (1 << NAVDATA_MAGNETO_TAG) |
        (1 << NAVDATA_TIME_TAG));

    stringstream ndOptionsString;
    ndOptionsString << ndOptions;
    setConfirmedConfigure(new AtCommandCONFIG("general:navdata_options", ndOptionsString.str()));
    // disable visual detection
    setConfirmedConfigure(new AtCommandCONFIG("detect:detect_type","3"));

    return true;
}

// ----------------- Public function ---------------- //

XciParrot::XciParrot(DataReceiver &dataReceiver, std::string ipAddress)
    : Xci(dataReceiver),
    atCommandSender_(atCommandQueue_, io_serviceCMD_, ipAddress),
    videoReceiver_(io_serviceVideo_, ipAddress),
    navdataReceiver_(dataReceiver, atCommandQueue_, state_, io_serviceNavdata_, ipAddress)
    //configurationReceiver_(atCommandQueue_,io_serviceCMD_,ipAddress)
{
    configuration_["XCI_PARAM_FP_PERSISTENCE"] = "50";
};

void XciParrot::init(){
    endAll_ = false;

    initNetwork();

    // init videoDecoder
    videoDecoder_.init(AV_CODEC_ID_H264);
    threadReadVideoData_ = std::move(std::thread(&XciParrot::processVideoData, this));

    setDefaultConfiguration();
}

std::string XciParrot::name() {
    return NAME;
}

SensorList XciParrot::sensorList() {
    SensorList sensorList;

    sensorList.push_back(Sensor("rotation", "ROTATION"));
    sensorList.push_back(Sensor("velocity", "VELOCITY"));

    sensorList.push_back(Sensor("altitudeAll", "ALTITUDE_ALL"));
    sensorList.push_back(Sensor("altitudeV", "ALTITUDE_V"));
    sensorList.push_back(Sensor("acceleration", "ACCELERATION"));
    sensorList.push_back(Sensor("gyro", "GYRO_RAW"));
    sensorList.push_back(Sensor("magneto", "MAGNETO_RAW"));
    sensorList.push_back(Sensor("internalTimeIMU", "TIME_LOC"));

    sensorList.push_back(Sensor("altitude", "ALTITUDE"));
    sensorList.push_back(Sensor("battery", "BATTERY"));
    sensorList.push_back(Sensor("video", "VIDEO"));
    sensorList.push_back(Sensor("internalTimeVideo", "TIME_LOC"));

    sensorList.push_back(Sensor("alive", "ALIVE"));

    return sensorList;
}

void* XciParrot::sensorData(const Sensor &sensor) {
    return NULL;
}

std::string XciParrot::configuration(const std::string &key) {
    if (configuration_.count(key) > 0){
        return configuration_[key];
    }
    else{
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
    CMDList.push_back("Reset");
    return CMDList;
}

void XciParrot::configuration(const std::string &key, const std::string &value) {
    atCommandQueue_.push(new AtCommandCONFIG(key, value));
}

void XciParrot::configuration(const InformationMap &configuration) {
    for (auto element : configuration) {
        atCommandQueue_.push(new AtCommandCONFIG(element.first, element.second));
    }
}

void XciParrot::command(const std::string &command) {
    if (command == "TakeOff") {
        atCommandQueue_.push(new AtCommandRef(STATE_TAKEOFF));
    } else if (command == "Land") {
        atCommandQueue_.push(new AtCommandRef(STATE_LAND));
    } else if (command == "EmegrencyStop") {
        if (!state_.getState(FLAG_ARDRONE_EMERGENCY_MASK)) {
            atCommandQueue_.push(new AtCommandRef(STATE_EMERGENCY));
        }
    } else if (command == "Normal") {
        if (state_.getState(FLAG_ARDRONE_EMERGENCY_MASK)) {
            atCommandQueue_.push(new AtCommandRef(STATE_NORMAL));
        }
    } else if (command == "Reset") {

    } else {

    }
}

void XciParrot::flyControl(float roll, float pitch, float yaw, float gaz) {
    //printf("Roll %f Pitch %f YAW %f GAZ %f \n", roll,pitch,yaw,gaz);
    if (std::abs(pitch) < EPSILON && std::abs(roll) < EPSILON) {
        atCommandQueue_.push(new AtCommandPCMD(DroneMove(roll, pitch, yaw, gaz)));
    } else {
        atCommandQueue_.push(new AtCommandPCMD(DroneMove(roll, pitch, yaw, gaz), false, false, true));
    }

}

XciParrot::~XciParrot() {
    endAll_ = true;

    // wait for atCMDThread end and then clear memory
    threadSendingATCmd_.join();
    threadReceiveNavData_.join();
    threadReadVideoData_.join();
    threadReadVideoReceiver_.join();

    // delete all AtCommand in queue
    while (atCommandQueue_.empty())
        delete atCommandQueue_.pop();
}

extern "C" {

Xci* CreateXci(DataReceiver &dataReceiver) {
    return new XciParrot(dataReceiver);
}
}
