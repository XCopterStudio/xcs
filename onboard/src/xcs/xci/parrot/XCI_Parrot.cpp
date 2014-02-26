#include <array>
#include <iostream>

#include "XCI_Parrot.hpp"
#include "video_encapsulation.h"
#include "video_decode.hpp"
#include <xcs/nodes/xobject/SyntacticTypes.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

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

const unsigned int XCI_Parrot::VIDEO_MAX_SIZE = 1024*1024;

// ----------------- Private function --------------- //

void XCI_Parrot::initNetwork() {
    boost::system::error_code ec;

    // connect to cmd port
    udp::endpoint parrotCMD(address::from_string("192.168.1.1"), PORT_CMD);
    socketCMD_ = new udp::socket(io_serviceCMD_);
    socketCMD_->connect(parrotCMD, ec);
    if (ec) {
        throw new ConnectionErrorException("Cannot connect command port.");
    }

    connectNavdata();

    // connect to video port
    tcp::endpoint parrotVideo(address::from_string("192.168.1.1"), PORT_VIDEO);
    socketVideo_ = new tcp::socket(io_serviceVideo_);
    socketVideo_->connect(parrotVideo, ec);
    if (ec) {
        throw new ConnectionErrorException("Cannot connect video port.");
    }
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
                socketCMD_->send(boost::asio::buffer(packetString.str(), packetString.str().size()));
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

void XCI_Parrot::receiveNavData() {
    uint8_t message[NAVDATA_MAX_SIZE];
    Navdata* navdata = (Navdata*) & message[0];
    size_t receiveSize = 0;

    initNavdataReceive();

    while (!endAll_) {
        receiveSize = socketData_->receive(boost::asio::buffer(message, NAVDATA_MAX_SIZE));
        if (navdata->sequence > sequenceNumberData_ && navdata->header == 0x55667788) { // all received data with sequence number lower then sequenceNumberData_ will be skipped.
            uint32_t navdataCks = NavdataProcess::computeChecksum(navdata, receiveSize);
            vector<OptionAcceptor*> options = NavdataProcess::parse(navdata, navdataCks ,receiveSize);
            if (options.size() > 0){
                processState(navdata->ardrone_state);
                processNavdata(options);
            }
            
            sequenceNumberData_ = navdata->sequence;
        }
    }
}

void XCI_Parrot::receiveVideo() {
    uint8_t* message = new uint8_t[VIDEO_MAX_SIZE];
    size_t receivedSize;
 
    while (!endAll_) {
        receivedSize = socketVideo_->receive(boost::asio::buffer(message, VIDEO_MAX_SIZE));
        parrot_video_encapsulation_t* videoPacket = (parrot_video_encapsulation_t*) & message[0];
        if(checkPaveSignature(videoPacket->signature) && checkFrameNumberAndType(videoPacket->frame_number,videoPacket->frame_type) && videoPacket->payload_size > 0){
            frameNumber_ = videoPacket->frame_number;

            AVPacket packet;
            packet.size = videoPacket->payload_size;
            packet.data = &message[videoPacket->header_size];
            if (videoDecoder_.decodeVideo(&packet)){

                AVFrame* frame = videoDecoder_.decodedFrame();
                BitmapType bitmapType;
                bitmapType.data = frame->data[0];
                bitmapType.height = frame->height;
                bitmapType.width = frame->width;

                dataReceiver_.notify("video", bitmapType);

                //cerr << "Decoded video frame " << videoPacket->frame_number << endl;
            }
        }
    }

    delete message;
}

bool XCI_Parrot::checkPaveSignature(uint8_t signature[4]){
    return signature[0] == 'P' && signature[1] == 'a' && signature[2] == 'V' && signature[3] == 'E';
}

bool XCI_Parrot::checkFrameNumberAndType(uint32_t number, uint8_t frameType){
    return (frameNumber_+1) == number || frameType == FRAME_TYPE_I_FRAME || frameType == FRAME_TYPE_IDR_FRAME;
}

void XCI_Parrot::connectNavdata(){
    // connect to navdata port
    udp::endpoint parrotData(address::from_string("192.168.1.1"), PORT_DATA);
    socketData_ = new udp::socket(io_serviceData_);

    boost::system::error_code ec;
    socketData_->connect(parrotData, ec);
    if (ec) {
        throw new ConnectionErrorException("Cannot connect navigation data port.");
    }
};

// function for navdata handling
void XCI_Parrot::initNavdataReceive() {
    // magic
    int32_t flag = 1; // 1 - unicast, 2 - multicast
    socketData_->send(boost::asio::buffer((uint8_t*) (&flag), sizeof (int32_t)));
}

void XCI_Parrot::processState(uint32_t droneState){
    state_.updateState(droneState);

    if (state_.getState(FLAG_ARDRONE_NAVDATA_BOOTSTRAP)) { //test if drone is in BOOTSTRAP MODE
        atCommandQueue_.push(new AtCommandCONFIG("general:navdata_demo", "TRUE")); // exit bootstrap mode and drone will send the demo navdata
    }

    if (state_.getState(FLAG_ARDRONE_COMMAND_MASK)){
        atCommandQueue_.push(new AtCommandCTRL(STATE_ACK_CONTROL_MODE));
    }

    if (state_.getState(FLAG_ARDRONE_COM_WATCHDOG_MASK)) { // reset sequence number
        sequenceNumberData_ = DEFAULT_SEQUENCE_NUMBER - 1;
        atCommandQueue_.push(new AtCommandCOMWDG());
    }

    if (state_.getState(FLAG_ARDRONE_COM_LOST_MASK)) { // TODO: check what exactly mean reinitialize the communication with the drone
        sequenceNumberData_ = DEFAULT_SEQUENCE_NUMBER - 1;
        initNavdataReceive();
    }
}

void XCI_Parrot::processNavdata(vector<OptionAcceptor*> &options) {
    OptionVisitor visitor(dataReceiver_);
    for(auto option : options){
        option->accept(visitor);
        delete option;
    }
}

NavdataOption* XCI_Parrot::getOption(NavdataOption* ptr, NavdataTag tag) {
    NavdataOption* ptrData = ptr;

    do {
        if (ptrData->size == 0) {
            return std::nullptr_t();
        } else {
            if (ptrData->tag == tag) {
                return ptrData;
            }

            ptrData = (NavdataOption*) (((uint8_t*) ptrData) + ptrData->size);
        }
    } while (true);
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

    std::array<char, 8192> buf;
    size_t size = socketComm.receive(boost::asio::buffer(buf.data(), buf.size()));

    std::string configuration(&buf[0],size);

    return configuration;
}

// ----------------- Public function ---------------- //

void XCI_Parrot::init() throw (ConnectionErrorException) {
    sequenceNumberCMD_ = DEFAULT_SEQUENCE_NUMBER;
    sequenceNumberData_ = DEFAULT_SEQUENCE_NUMBER - 1;
    frameNumber_ = 0;

    endAll_ = false;

    std::cerr << "Before network" << std::endl;
    initNetwork();
    std::cerr << "After network" << std::endl;

    // init videoDecoder
    videoDecoder_.init(AV_CODEC_ID_H264);

    // start all threads
    threadSendingATCmd_ = std::move(std::thread(&XCI_Parrot::sendingATCommands, this));
    threadReceiveNavData_ = std::move(std::thread(&XCI_Parrot::receiveNavData, this));
    threadReceiveVideo_ = std::move(std::thread(&XCI_Parrot::receiveVideo, this));
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // workaround as Parrot needs some time before another command can be send to it
    std::cerr << "After threads" << std::endl;
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

    sensorList.push_back(Sensor("phi","phi"));
    sensorList.push_back(Sensor("theta","theta"));
    sensorList.push_back(Sensor("psi","psi"));
    sensorList.push_back(Sensor("altitude","altitude"));
    sensorList.push_back(Sensor("video", "video"));

    return sensorList;
}

ParameterValueType XCI_Parrot::parameter(ParameterNameType name) {
    switch(name) {
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
        if (!state_.getState(FLAG_ARDRONE_EMERGENCY_MASK)){
            atCommandQueue_.push(new AtCommandRef(STATE_EMERGENCY));
        }
    } else if (command == "Normal") {
        if (state_.getState(FLAG_ARDRONE_EMERGENCY_MASK)){
            atCommandQueue_.push(new AtCommandRef(STATE_NORMAL));
        }
    } else if (command == "Reset") {

    } else {

    }
}

void XCI_Parrot::flyParam(float roll, float pitch, float yaw, float gaz) {
    //printf("Roll %f Pitch %f YAW %f GAZ %f \n", roll,pitch,yaw,gaz);
    if(std::abs(pitch) < EPSILON && std::abs(roll) < EPSILON){
        atCommandQueue_.push(new AtCommandPCMD(DroneMove(roll, pitch, yaw, gaz)));    
    }else{
        atCommandQueue_.push(new AtCommandPCMD(DroneMove(roll, pitch, yaw, gaz),false,false,true));    
    }
    
}

XCI_Parrot::~XCI_Parrot() {
    endAll_ = true;

    //delete all socket
    delete socketCMD_;
    delete socketData_;
    delete socketVideo_;
    // wait for atCMDThread end and then clear memory
    threadSendingATCmd_.join();
    threadReceiveNavData_.join();
    threadReceiveVideo_.join();

    // delete all AtCommand in queue
    while (atCommandQueue_.empty())
        delete atCommandQueue_.pop();
}

extern "C" {
    XCI* CreateXci(DataReceiver &dataReceiver) { 
        return new XCI_Parrot(dataReceiver); 
    }
}
