#include <array>
#include <iostream>

#include "XCI_Parrot.hpp"
#include "video_encapsulation.h"
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace xci_parrot;
// ----------------- Constant ----------------------- //

const int XCI_Parrot::PORT_COM = 5559;
const int XCI_Parrot::PORT_CMD = 5556;
const int XCI_Parrot::PORT_VIDEO = 5555;
const int XCI_Parrot::PORT_DATA = 5554;

const float XCI_Parrot::EPSILON = 1.0e-10;

const unsigned int XCI_Parrot::AT_CMD_PACKET_SIZE = 1024;

const std::string XCI_Parrot::NAME = "Parrot AR Drone 2.0 XCI";

const int XCI_Parrot::DEFAULT_SEQUENCE_NUMBER = 1;

const unsigned int XCI_Parrot::VIDEO_MAX_SIZE = 3686400;

// ----------------- Private function --------------- //

void XCI_Parrot::initNetwork() {
    boost::system::error_code ec;

    // connect to cmd port
    udp::endpoint parrotCMD(address::from_string("192.168.1.1"), PORT_CMD);
    socketCMD = new udp::socket(io_serviceCMD);
    socketCMD->connect(parrotCMD, ec);
    if (ec) {
        throw new ConnectionErrorException("Cannot connect command port.");
    }

    // connect to navdata port
    udp::endpoint parrotData(address::from_string("192.168.1.1"), PORT_DATA);
    socketData = new udp::socket(io_serviceData);
    socketData->connect(parrotData, ec);
    if (ec) {
        throw new ConnectionErrorException("Cannot connect navigation data port.");
    }

    // connect to video port
    tcp::endpoint parrotVideo(address::from_string("192.168.1.1"), PORT_VIDEO);
    socketVideo = new tcp::socket(io_serviceVideo);
    socketVideo->connect(parrotVideo, ec);
    if (ec) {
        throw new ConnectionErrorException("Cannot connect video port.");
    }
}

void XCI_Parrot::sendingATCommands() {
    std::stringstream packetString;
    unsigned int counter = 0;

    while (!endAll) { // EndAll is true when instance of XCI_Parrot is in destructor.
        atCommand* cmd;
        if (atCommandQueue.tryPop(cmd)) {
            counter = 0;
            std::string cmdString = cmd->toString(sequenceNumberCMD++);
            printf("%s\n", cmdString.c_str());
            delete cmd;

            unsigned int newSize = packetString.str().size() + cmdString.size() + 1; // one for new line 
            if (newSize > AT_CMD_PACKET_SIZE || atCommandQueue.empty()) { // send prepared packet
                socketCMD->send(boost::asio::buffer(packetString.str(), packetString.str().size()));
                // clear packet string
                packetString.str(std::string());
                packetString.clear();
            }

            // add end of line at the end of each atCommand
            if (packetString.str().size() > 0) {
                packetString << std::endl;
            }

            packetString << cmdString;
        } else { // We haven't nothing to send. Put thread to sleep on some time.
            /*if(++counter > 50){
                atCommandQueue.push(new atCommandCOMWDG());
                counter=0;
            }*/
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    // end thread
}

void XCI_Parrot::receiveNavData() {
    uint8_t message[NAVDATA_MAX_SIZE];
    navdata_t* navdata = (navdata_t*) & message[0];
    size_t receiveSize = 0;

    initNavdataReceive();

    while (!endAll) {
        receiveSize = socketData->receive(boost::asio::buffer(message, NAVDATA_MAX_SIZE));
        if (navdata->sequence > sequenceNumberData) { // all received data with sequence number lower then sequenceNumberData will be skipped.
            if (isCorrectData(navdata, receiveSize)) { // test correctness of received data
                processReceivedNavData(navdata, receiveSize);
                sequenceNumberData = navdata->sequence;
            }
        }
    }
}

void XCI_Parrot::receiveVideo() {
    uint8_t* message = new uint8_t[VIDEO_MAX_SIZE];
    size_t receivedSize;
    videoDecoder.init(AV_CODEC_ID_H264);
    while (!endAll) {
        receivedSize = socketVideo->receive(boost::asio::buffer(message, VIDEO_MAX_SIZE));
        parrot_video_encapsulation_t* videoPacket = (parrot_video_encapsulation_t*) & message[0];
        if (videoPacket->signature[0] == 'P' && videoPacket->signature[1] == 'a' && videoPacket->signature[2] == 'V' && videoPacket->signature[3] == 'E') {
            AVPacket avpacket;
            avpacket.size = videoPacket->payload_size;
            avpacket.data = &message[videoPacket->header_size];
            //videoDecoder.decodeVideo(std::nullptr_t(),&avpacket);
        }
    }

    delete message;
}

// function for navdata handling

void XCI_Parrot::initNavdataReceive() {
    // magic
    int32_t flag = 1; // 1 - unicast, 2 - multicast
    socketData->send(boost::asio::buffer((uint8_t*) (&flag), sizeof (int32_t)));
}

bool XCI_Parrot::isCorrectData(navdata_t* navdata, const size_t size) { // simple check: only sum all data to uint32_t and then compare with value in checksum option
    uint8_t* data = (uint8_t*) navdata;
    uint32_t checksum = 0;
    size_t dataSize = size - sizeof (navdata_cks_t);

    for (unsigned int i = 0; i < dataSize; i++) {
        checksum += (uint32_t) data[i];
    }

    navdata_cks_t* navdataChecksum = (navdata_cks_t*) getOption(&navdata->options[0], NAVDATA_CKS_TAG);
    if (navdataChecksum == std::nullptr_t())
        return false;

    return navdataChecksum->cks == checksum;
}

void XCI_Parrot::processReceivedNavData(navdata_t* navdata, const size_t size) {
    state.updateState(navdata->ardrone_state);

    //printf("Drone state %x, watchdog %i \n",navdata->ardrone_state,state.getState(ARDRONE_COM_WATCHDOG_MASK));

    if (state.getState(ARDRONE_NAVDATA_BOOTSTRAP)) { //test if drone is in BOOTSTRAP MODE
        atCommandQueue.push(new atCommandCONFIG("general:navdata_demo", "TRUE")); // exit bootstrap mode and drone will send the demo navdata
    }

    if(state.getState(ARDRONE_COMMAND_MASK)){
        atCommandQueue.push(new atCommandCTRL(ACK_CONTROL_MODE));
    }

    if (state.getState(ARDRONE_COM_WATCHDOG_MASK)) { // reset sequence number
        sequenceNumberData = DEFAULT_SEQUENCE_NUMBER - 1;
        atCommandQueue.push(new atCommandCOMWDG());
    }

    if (state.getState(ARDRONE_COM_LOST_MASK)) { // TODO: check what exactly mean reinitialize the communication with the drone
        sequenceNumberData = DEFAULT_SEQUENCE_NUMBER - 1;
        initNavdataReceive();
    }


    //TODO: parse options from ardrone!!!
}

navdata_option_t* XCI_Parrot::getOption(navdata_option_t* ptr, navdata_tag_t tag) {
    navdata_option_t* ptr_data = ptr;

    do {
        if (ptr_data->size == 0) {
            return std::nullptr_t();
        } else {
            if (ptr_data->tag == tag) {
                return ptr_data;
            }

            ptr_data = (navdata_option_t*) (((uint8_t*) ptr_data) + ptr_data->size);
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

    atCommandQueue.push(new atCommandCTRL(CFG_GET_CONTROL_MODE));

    std::array<char, 8192> buf;
    size_t size = socketComm.receive(boost::asio::buffer(buf.data(), buf.size()));

    std::string configuration(&buf[0],size);

    return configuration;
}

// ----------------- Public function ---------------- //

void XCI_Parrot::init() throw (ConnectionErrorException) {
    sequenceNumberCMD = DEFAULT_SEQUENCE_NUMBER;
    sequenceNumberData = DEFAULT_SEQUENCE_NUMBER - 1;

    endAll = false;

    initNetwork();

    // start all threads
    sendingATCmdThread = std::move(std::thread(&XCI_Parrot::sendingATCommands, this));
    receiveNavDataThread = std::move(std::thread(&XCI_Parrot::receiveNavData, this));
    receiveVideoThread = std::move(std::thread(&XCI_Parrot::receiveVideo, this));
}

void XCI_Parrot::reset() {

}

void XCI_Parrot::start() {
    while (!state.getState(ARDRONE_FLY_MASK)) {
        atCommandQueue.push(new atCommandRef(TAKEOFF));
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }
}

void XCI_Parrot::stop() {
    while (state.getState(ARDRONE_FLY_MASK)) {
        atCommandQueue.push(new atCommandRef(LAND));
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }
}

std::string XCI_Parrot::getName() {
    return NAME;
}

sensorList XCI_Parrot::getSensorList() {
    return sensorList();
}

void* XCI_Parrot::getSensorData(const Sensor &sensor) {
    return NULL;
}

std::string XCI_Parrot::getConfiguration(const std::string &key) {
    return "";
}

informationMap XCI_Parrot::getConfiguration() {
    downloadConfiguration();
    return informationMap();
}

specialCMDList XCI_Parrot::getSpecialCMD() {
    specialCMDList CMDList;
    CMDList.push_back("TakeOff");
    CMDList.push_back("Land");
    CMDList.push_back("EmergencyStop");
    CMDList.push_back("Normal");
    CMDList.push_back("Reset");
    return CMDList;
}

int XCI_Parrot::setConfiguration(const std::string &key, const std::string &value) {
    atCommandQueue.push(new atCommandCONFIG(key, value));
    return 1;
}

int XCI_Parrot::setConfiguration(const informationMap &configuration) {
    for (auto element : configuration) {
        atCommandQueue.push(new atCommandCONFIG(element.first, element.second));
    }
    return 1;
}

// TODO: update according to the ardrone state

void XCI_Parrot::sendCommand(const std::string &command) {
    if (command == "TakeOff") {
        while (!state.getState(ARDRONE_FLY_MASK)) {
            atCommandQueue.push(new atCommandRef(TAKEOFF));
            std::this_thread::sleep_for(std::chrono::milliseconds(25));
        }
    } else if (command == "Land") {
        while (state.getState(ARDRONE_FLY_MASK)) {
            atCommandQueue.push(new atCommandRef(LAND));
            std::this_thread::sleep_for(std::chrono::milliseconds(25));
        }
    } else if (command == "EmegrencyStop") {
        if(!state.getState(ARDRONE_EMERGENCY_MASK)){
            atCommandQueue.push(new atCommandRef(EMERGENCY));
        }
    } else if (command == "Normal") {
        if(state.getState(ARDRONE_EMERGENCY_MASK)){
            atCommandQueue.push(new atCommandRef(NORMAL));
        }
    } else if (command == "Reset") {

    } else {

    }
}

void XCI_Parrot::sendFlyParam(float roll, float pitch, float yaw, float gaz) {
    //printf("Roll %f Pitch %f YAW %f GAZ %f \n", roll,pitch,yaw,gaz);
    if(std::abs(pitch) < EPSILON && std::abs(roll) < EPSILON){
        atCommandQueue.push(new atCommandPCMD(droneMove(roll, pitch, yaw, gaz)));    
    }else{
        atCommandQueue.push(new atCommandPCMD(droneMove(roll, pitch, yaw, gaz),false,false,true));    
    }
    
}

XCI_Parrot::~XCI_Parrot() {
    endAll = true;

    //delete all socket
    delete socketCMD;
    delete socketData;
    delete socketVideo;
    // wait for atCMDThread end and then clear memory
    sendingATCmdThread.join();
    receiveNavDataThread.join();
    receiveVideoThread.join();

    // delete all atCommand in queue
    while (atCommandQueue.empty())
        delete atCommandQueue.pop();
}
// TODO this should be always compiled as a library
//int main() {
//    XCI_Parrot parrot;
//    parrot.init();
//
//    parrot.start();
//    std::this_thread::sleep_for(std::chrono::seconds(6));
//    parrot.stop();
//
//    while (true) {
//        std::this_thread::sleep_for(std::chrono::seconds(1));
//    };
//}


extern "C" {
    XCI* CreateXci() { return new XCI_Parrot(); }
}
