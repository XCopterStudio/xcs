#include "XCI_Parrot.hpp"

using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace xci_parrot;
// ----------------- Constant ----------------------- //

const int XCI_Parrot::CMDPort = 5556;
const int XCI_Parrot::VideoPort = 5555;
const int XCI_Parrot::DataPort = 5554;

const unsigned int XCI_Parrot::atCMDPacketSize = 1024;

const std::string XCI_Parrot::name ="Parrot AR Drone 2.0 XCI";

const int XCI_Parrot::defaultSequenceNumber = 1;

// ----------------- Private function --------------- //

void XCI_Parrot::initNetwork(){
	boost::system::error_code ec;

	// connect to cmd port
	udp::endpoint parrotCMD = udp::endpoint(address::from_string("192.168.1.1"),CMDPort);
	socketCMD = new udp::socket(io_service);
	socketCMD->connect(parrotCMD,ec);
	if(ec){
		printf("Error \n");
	}

	// connect to navdata port
	udp::endpoint parrotData = udp::endpoint(address::from_string("192.168.1.1"),DataPort);
	socketData = new udp::socket(io_service);
	socketData->connect(parrotData,ec);
	if(ec){
		printf("Error \n");
	}

	// connect to video port
	tcp::endpoint parrotVideo = tcp::endpoint(address::from_string("192.168.1.1"),VideoPort);
	socketVideo = new tcp::socket(io_service);
	socketVideo->connect(parrotVideo,ec);
	if(ec){
		printf("Error \n");
	}
}

void XCI_Parrot::sendingATCommands(){
	std::stringstream packetString;

	while(!endAll){ // EndAll is true when instance of XCI_Parrot is in destructor.
		atCommand* cmd;
		if(atCommandQueue.tryPop(cmd)){
			std::string cmdString = cmd->toString(sequenceNumberCMD++);
			delete cmd;

			unsigned int newSize = packetString.str().size() + cmdString.size() + 1; // one for new line 
			if(newSize > atCMDPacketSize || atCommandQueue.empty()){ // send prepared packet
				socketCMD->send(boost::asio::buffer(packetString.str(),packetString.str().size()));

				// clear packet string
				packetString.str(std::string());
				packetString.clear();
			}

			// add end of line at the end of each atCommand
			if(packetString.str().size() > 0){
				packetString << std::endl;
			}

			packetString << cmdString;
		}else{ // We haven't nothing to send. Put thread to sleep on some time.
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
		}
	}

	// end thread
}

void XCI_Parrot::receiveNavData(){
  uint8_t message[NAVDATA_MAX_SIZE];
  navdata_t* navdata = (navdata_t*) &message[0];
	size_t receiveSize = 0;

	initNavdataReceive();

  while(!endAll){
    receiveSize = socketData->receive(boost::asio::buffer(message,NAVDATA_MAX_SIZE));
		if(navdata->sequence < sequenceNumberData){ // all received data with sequence number lower then sequenceNumberData will be skipped.
			if(isDataCorrect(navdata,receiveSize)){ // test correctness of received data
				processReceivedNavData(navdata, receiveSize);
			}
		}
  }
}

void XCI_Parrot::receiveVideo(){
	while(!endAll){
		
	}
}

// function for navdata handling
void XCI_Parrot::initNavdataReceive(){
	// magic
  int32_t flag = 1; // 1 - unicast, 2 - multicast
  socketData->send(boost::asio::buffer((uint8_t*)(&flag),sizeof(int32_t)));
}

bool XCI_Parrot::isDataCorrect(navdata_t* navdata, const size_t size){ // simple check: only sum all data to uint32_t and then compare with value in checksum option
	uint8_t* data = (uint8_t*) navdata;
	uint32_t checksum;
	size_t dataSize = size - sizeof(navdata_cks_t);

	for(unsigned int i = 0; i < dataSize; i++){
		checksum += data[i];
	}

	navdata_cks_t* navdataChecksum = (navdata_cks_t*)&navdata[dataSize];

	return navdataChecksum->cks == checksum;
}

void XCI_Parrot::processReceivedNavData(navdata_t* navdata, const size_t size){
	state.updateState(navdata->ardrone_state);
	if(navdata->header == NAVDATA_HEADER){ // test god type of navdata
		if(state.getState(ARDRONE_NAVDATA_BOOTSTRAP)){ //test if drone is in BOOTSTRAP MODE
			atCommandQueue.push(new atCommandCONFIG("general:navdata_demo","TRUE")); // exit bootstrap mode and drone will send the demo navdata
		}else{
		if(state.getState(ARDRONE_COM_WATCHDOG_MASK)){ // reset sequence number
			sequenceNumberData = defaultSequenceNumber - 1;
			atCommandQueue.push(new atCommandCOMWDG());
		}else{
		if(state.getState(ARDRONE_COM_LOST_MASK)){ // TODO: check what exactly mean reinitialize the communication with the drone
			sequenceNumberData = defaultSequenceNumber;
			initNavdataReceive();
		}}}

	}
}

// ----------------- Public function ---------------- //

void XCI_Parrot::init(){
	sequenceNumberCMD = defaultSequenceNumber;
	sequenceNumberVideo = defaultSequenceNumber;
	sequenceNumberData = defaultSequenceNumber;

	endAll = false;

  initNetwork();

	// start all threads
	sendingATCmdThread = std::move(std::thread(&XCI_Parrot::sendingATCommands,this));
  receiveNavDataThread = std::move(std::thread(&XCI_Parrot::receiveNavData, this));
	receiveVideoThread = std::move(std::thread(&XCI_Parrot::receiveVideo, this));
}

void XCI_Parrot::reset(){

}

void XCI_Parrot::start(){

}

void XCI_Parrot::stop(){

}

std::string XCI_Parrot::getName(){
	return name;
}

sensorList XCI_Parrot::getSensorList(){
	return sensorList();
}

void* XCI_Parrot::getSensorData(const Sensor &sensor){
	return NULL;
}

std::string XCI_Parrot::getConfiguration(const std::string &key){
	return "";
}

informationMap XCI_Parrot::getConfiguration(){
	return informationMap();
}

specialCMDList XCI_Parrot::getSpecialCMD(){
	specialCMDList CMDList;
	CMDList.push_back("TakeOff");
	CMDList.push_back("Land");
	CMDList.push_back("EmergencyStop");
	CMDList.push_back("Normal");
	CMDList.push_back("Reset");
	return CMDList;
}
	
int XCI_Parrot::setConfiguration(const std::string &key, const std::string &value){
	atCommandQueue.push(new atCommandCONFIG(key, value));
	return 1;
}

int XCI_Parrot::setConfiguration(const informationMap &configuration){
	for(auto element : configuration){
		atCommandQueue.push(new atCommandCONFIG(element.first,element.second));
	}
	return 1;
}

// TODO: update according to the ardrone state
void XCI_Parrot::sendCommand(const std::string &command){
	if(command == "TakeOff"){
		atCommandQueue.push(new atCommandRef(TAKEOFF));
	}else{ if(command == "Land"){
		atCommandQueue.push(new atCommandRef(LAND));
	}else{ if(command == "EmegrencyStop"){
		atCommandQueue.push(new atCommandRef(EMERGENCY));
	}else{ if(command == "Normal"){
		atCommandQueue.push(new atCommandRef(NORMAL));
	}else{ if(command == "Reset"){
		
	}else{
		
	}}}}}
}

void XCI_Parrot::sendFlyParam(float roll, float pitch, float yaw, float gaz){
	atCommandQueue.push(new atCommandPCMD(droneMove(roll,pitch,yaw,gaz)));
}

XCI_Parrot::~XCI_Parrot(){
	endAll = true;

	// wait for atCMDThread end and then clear memory
	sendingATCmdThread.join();	
	receiveNavDataThread.join();
	receiveVideoThread.join();

	// delete all atCommand in queue
	while(atCommandQueue.empty())
		delete atCommandQueue.pop();
}


int main(){
	XCI_Parrot parrot;
	parrot.init();

  while(true){
  };
}

