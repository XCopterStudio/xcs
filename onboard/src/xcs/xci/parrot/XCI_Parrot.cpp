#include <array>
#include <iostream>
#include <cstring>

#include "XCI_Parrot.hpp"
#include "video_encapsulation.h"
#include "video_decode.hpp"
#include <xcs/nodes/xobject/syntactic_types.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace xcs::xci;
using namespace xcs::nodes; // because of syntactic types
using namespace xcs::xci::parrot;
// ----------------- Constant ----------------------- //

const int XCI_Parrot::PORT_COM = 5559;
const int XCI_Parrot::PORT_CMD = 5556;
const int XCI_Parrot::PORT_VIDEO = 5555;
const int XCI_Parrot::PORT_DATA = 5554;

const float XCI_Parrot::EPSILON = (float) 1.0e-10;

const unsigned int XCI_Parrot::AT_CMD_PACKET_SIZE = 1024;

const std::string XCI_Parrot::NAME = "Parrot AR Drone 2.0 XCI";

const int32_t XCI_Parrot::DEFAULT_SEQUENCE_NUMBER = 1;

// ----------------- Private function --------------- //

void XCI_Parrot::initNetwork() {
    boost::system::error_code ec;

    // connect to cmd port
	socketCMD_.open(udp::v4());
    socketCMD_.connect(parrotCMD_, ec);
    if (ec) {
        throw new ConnectionErrorException("Cannot connect command port.");
    }
    threadSendingATCmd_ = std::move(std::thread(&XCI_Parrot::sendingATCommands, this));

	// connect to video port
    navdataReceiver_.connect();
	videoReceiver_.connect();

	threadReadVideoReceiver_ = std::move(std::thread(boost::bind(&boost::asio::io_service::run, &io_serviceVideo_)));
	threadReceiveNavData_ = std::move(std::thread(boost::bind(&boost::asio::io_service::run, &io_serviceData_)));
}

void XCI_Parrot::sendingATCommands() {
    std::stringstream packetString;
    unsigned int counter = 0;

    while (!endAll_) { // EndAll is true when instance of XCI_Parrot is in destructor.
        AtCommand* cmd;
        if (atCommandQueue_.tryPop(cmd)) {
            counter = 0;
            std::string cmdString = cmd->toString(sequenceNumberCMD_++);
            //printf("%s\n", cmdString.c_str());
            delete cmd;

            unsigned int newSize = packetString.str().size() + cmdString.size() + 1; // one for new line 
            if (newSize > AT_CMD_PACKET_SIZE || atCommandQueue_.empty()) { // send prepared packet
                socketCMD_.send(boost::asio::buffer(packetString.str(), packetString.str().size()));
                // clear packet string
                packetString.str(std::string());
                packetString.clear();
            }

            // add end of line at the end of each AtCommand
            if (packetString.str().size() > 0) {
                packetString << std::endl;
            }

            packetString << cmdString;
        } else { // We haven't nothing to send. Put thread to sleep on some time.
            /*if(++counter > 50){
                atCommandQueue_.push(new AtCommandCOMWDG());
                counter=0;
            }*/
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    // end thread
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
                BitmapType bitmap;
                bitmap.data = avFrame->data[0];
                bitmap.height = avFrame->height;
                bitmap.width = avFrame->width;
                dataReceiver_.notify("video",bitmap);
            }
            delete frame;
        }
        else{
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}


// TODO: set timeout for receive!!!

std::string XCI_Parrot::downloadConfiguration() throw (ConnectionErrorException) {
    boost::system::error_code ec;

    boost::asio::io_service io_service;
    tcp::socket socketComm(io_service);
    tcp::endpoint parrotComm(address::from_string("192.168.1.1"), PORT_COM);
    socketComm.connect(parrotComm, ec);
    if (ec) {
        throw new ConnectionErrorException("Cannot connect communication port.");
    }

    atCommandQueue_.push(new AtCommandCTRL(STATE_CFG_GET_CONTROL_MODE));

    std::string configuration;
    std::array<char, 8192> buf;
    size_t size(0);
    size_t tries = 2;

    // TODO doesn't work
    //    socketComm.async_receive(boost::asio::buffer(buf.data(), buf.size()), [buf](const boost::system::error_code& error, size_t bytes_transferred) {
    //        std::string stringBuffer(buf.data(), bytes_transferred);
    //        cerr << "[async] " << stringBuffer << endl;
    //
    //    });
    while (tries > 0 && (size = socketComm.receive(boost::asio::buffer(buf.data(), buf.size())))) {
        std::string stringBuffer(buf.data(), size);
        cerr << stringBuffer << endl;
        configuration += stringBuffer;
        tries--;
    }
    return configuration;
}

// ----------------- Public function ---------------- //

void XCI_Parrot::init() throw (ConnectionErrorException) {
    boost::log::core::get()->set_filter
            (
            boost::log::trivial::severity >= boost::log::trivial::debug
            );

    sequenceNumberCMD_ = DEFAULT_SEQUENCE_NUMBER;

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

    sensorList.push_back(Sensor("phi", "phi"));
    sensorList.push_back(Sensor("theta", "theta"));
    sensorList.push_back(Sensor("psi", "psi"));
    sensorList.push_back(Sensor("altitude", "altitude"));
    sensorList.push_back(Sensor("video", "video"));

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
    downloadConfiguration();
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
        while (!state_.getState(FLAG_ARDRONE_FLY_MASK)) {
            atCommandQueue_.push(new AtCommandRef(STATE_TAKEOFF));
            std::this_thread::sleep_for(std::chrono::milliseconds(25));
        }
    } else if (command == "Land") {
        while (state_.getState(FLAG_ARDRONE_FLY_MASK)) {
            atCommandQueue_.push(new AtCommandRef(STATE_LAND));
            std::this_thread::sleep_for(std::chrono::milliseconds(25));
        }
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

    //delete all socket
    socketCMD_.close();
    socketData_.close();
    // wait for atCMDThread end and then clear memory
    threadSendingATCmd_.join();
    threadReceiveNavData_.join();

    // delete all AtCommand in queue
    while (atCommandQueue_.empty())
        delete atCommandQueue_.pop();
}

extern "C" {

XCI* CreateXci(DataReceiver &dataReceiver) {
    return new XCI_Parrot(dataReceiver);
}
}
