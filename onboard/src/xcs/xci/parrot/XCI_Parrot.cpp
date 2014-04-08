#include <array>
#include <iostream>
#include <cstring>

#include "XCI_Parrot.hpp"
#include "video_encapsulation.h"
#include "video_decode.hpp"
#include <xcs/types/bitmap_type.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

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
            avPacket.data = &frame->data[frame->payload_offset];
            avPacket.size = frame->payload_size - frame->payload_offset;
            if (videoDecoder_.decodeVideo(&avPacket)){
                AVFrame* avFrame = videoDecoder_.decodedFrame();
                BitmapType bitmap(avFrame->width, avFrame->height, avFrame->data[0]);
                dataReceiver_.notify("video",bitmap);
            }
            //delete frame;
        }
        else{
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}

// ----------------- Public function ---------------- //

void XCI_Parrot::init(){
    boost::log::core::get()->set_filter
            (
            boost::log::trivial::severity >= boost::log::trivial::debug
            );

    endAll_ = false;

    std::cerr << "Before network" << std::endl;
    initNetwork();
    std::cerr << "After network" << std::endl;

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

    sensorList.push_back(Sensor("altitude", "ALTITUDE"));
    sensorList.push_back(Sensor("battery", "BATTERY"));
    sensorList.push_back(Sensor("video", "VIDEO"));

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
    unsigned int i;
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

void XCI_Parrot::flyParam(float roll, float pitch, float yaw, float gaz) {
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
