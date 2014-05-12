#include "XCI_Parrot.hpp"

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

const float XCI_Parrot::EPSILON = (float) 1.0e-10;

const std::string XCI_Parrot::NAME = "Parrot AR Drone 2.0 XCI";

// ----------------- Private function --------------- //

void XCI_Parrot::initNetwork() {
	// connect to video port
    atCommandSender_.connect();
    videoReceiver_.connect();
    navdataReceiver_.connect();

    threadSendingATCmd_ = std::move(std::thread(boost::bind(&boost::asio::io_service::run, &io_serviceCMD_)));
	threadReadVideoReceiver_ = std::move(std::thread(boost::bind(&boost::asio::io_service::run, &io_serviceVideo_)));
	threadReceiveNavData_ = std::move(std::thread(boost::bind(&boost::asio::io_service::run, &io_serviceNavdata_)));
}

void XCI_Parrot::processVideoData(){
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
                BitmapTypeChronologic bitmap(avFrame->width, avFrame->height, avFrame->data[0],frame->timestamp);
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

bool XCI_Parrot::setConfirmedConfigure(AtCommand *command){

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

bool XCI_Parrot::setDefaultConfiguration(){
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

bool XCI_Parrot::setNavdataReceive(bool full_mode){

    unsigned int count = 0;

    do{
        if (full_mode){
            atCommandQueue_.push(new AtCommandCONFIG("general:navdata_demo", "FALSE")); // exit bootstrap mode and drone will send the demo navdata
        }
        else{
            atCommandQueue_.push(new AtCommandCONFIG("general:navdata_demo", "TRUE")); // exit bootstrap mode and drone will send the demo navdata
        }

        this_thread::sleep_for(std::chrono::milliseconds(10));

        if(++count >= 20){
            XCS_LOG_WARN("Cannot receive general:navdata_demo ack.");;
            return false;
        }

    } while (!state_.getState(FLAG_ARDRONE_COMMAND_MASK));

    count = 0;
    do{
        atCommandQueue_.push(new AtCommandCTRL(STATE_ACK_CONTROL_MODE));
        this_thread::sleep_for(std::chrono::milliseconds(10));

        if (++count >= 20){
            XCS_LOG_WARN("Cannot receive clear general:navdata_demo ack.");
            return false;
        }
    } while (state_.getState(FLAG_ARDRONE_COMMAND_MASK));

    return true;
}

// ----------------- Public function ---------------- //

void XCI_Parrot::init(){
    endAll_ = false;

    XCS_LOG_INFO("Before network");
    initNetwork();
    XCS_LOG_INFO("After network");

    setDefaultConfiguration();
    //setNavdataReceive(true);

    // init videoDecoder
    videoDecoder_.init(AV_CODEC_ID_H264);
    threadReadVideoData_ = std::move(std::thread(&XCI_Parrot::processVideoData, this));
}

void XCI_Parrot::reset() {

}

void XCI_Parrot::start() {
    while (!state_.getState(FLAG_ARDRONE_FLY_MASK)) {
        atCommandQueue_.push(new AtCommandRef(STATE_TAKEOFF));
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }
}

void XCI_Parrot::stop() {
    while (state_.getState(FLAG_ARDRONE_FLY_MASK)) {
        atCommandQueue_.push(new AtCommandRef(STATE_LAND));
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }
}

std::string XCI_Parrot::name() {
    return NAME;
}

SensorList XCI_Parrot::sensorList() {
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

ParameterValueType XCI_Parrot::parameter(ParameterNameType name) {
    switch (name) {
        case XCI_PARAM_FP_PERSISTENCE:
            return "30";
        default:
            throw std::runtime_error("Parameter not defined.");
    }
}

void* XCI_Parrot::sensorData(const Sensor &sensor) {
    return NULL;
}

std::string XCI_Parrot::configuration(const std::string &key) {
    return "";
}

InformationMap XCI_Parrot::configuration() {
    //configurationReceiver_.getConfiguration();

    return InformationMap();
}

SpecialCMDList XCI_Parrot::specialCMD() {
    SpecialCMDList CMDList;
    CMDList.push_back("TakeOff");
    CMDList.push_back("Land");
    CMDList.push_back("EmergencyStop");
    CMDList.push_back("Normal");
    CMDList.push_back("Reset");
    return CMDList;
}

void XCI_Parrot::configuration(const std::string &key, const std::string &value) {
    atCommandQueue_.push(new AtCommandCONFIG(key, value));
}

void XCI_Parrot::configuration(const InformationMap &configuration) {
    for (auto element : configuration) {
        atCommandQueue_.push(new AtCommandCONFIG(element.first, element.second));
    }
}

void XCI_Parrot::command(const std::string &command) {
    if (command == "TakeOff") {
        atCommandQueue_.push(new AtCommandRef(STATE_TAKEOFF));
        /*if (!state_.getState(FLAG_ARDRONE_FLY_MASK)){
            atCommandQueue_.push(new AtCommandRef(STATE_TAKEOFF));
        }*/
        /*for (i = 0; i < TRY_COUNT; ++i) {
            atCommandQueue_.push(new AtCommandRef(STATE_TAKEOFF));
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            if (state_.getState(FLAG_ARDRONE_FLY_MASK)){
                break;
            }
        }*/
    } else if (command == "Land") {
        atCommandQueue_.push(new AtCommandRef(STATE_LAND));
       /* if (state_.getState(FLAG_ARDRONE_FLY_MASK)){
            atCommandQueue_.push(new AtCommandRef(STATE_LAND));
        }*/
        /*for (i = 0; i < TRY_COUNT; ++i) {
            atCommandQueue_.push(new AtCommandRef(STATE_LAND));
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            if (!state_.getState(FLAG_ARDRONE_FLY_MASK)) {
                break;
            }
        }*/
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

void XCI_Parrot::flyControl(float roll, float pitch, float yaw, float gaz) {
    //printf("Roll %f Pitch %f YAW %f GAZ %f \n", roll,pitch,yaw,gaz);
    if (std::abs(pitch) < EPSILON && std::abs(roll) < EPSILON) {
        atCommandQueue_.push(new AtCommandPCMD(DroneMove(roll, pitch, yaw, gaz)));
    } else {
        atCommandQueue_.push(new AtCommandPCMD(DroneMove(roll, pitch, yaw, gaz), false, false, true));
    }

}

XCI_Parrot::~XCI_Parrot() {
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

XCI* CreateXci(DataReceiver &dataReceiver) {
    return new XCI_Parrot(dataReceiver);
}
}
