#include "XCI_Parrot.hpp"
#include "AT_Command.hpp"
#include "navdata_common.h"

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
	while(!endAll){ 
		atCommand* cmd;
		if(atCommandQueue.tryPop(cmd)){
			std::string cmdString = cmd->toString(sequenceNumberCMD++);
			delete cmd;

			unsigned int newSize = packetString.str().size() + cmdString.size() + 1; // one for new line 
			if(newSize > atCMDPacketSize || atCommandQueue.empty()){ // send packet
				socketCMD->send(boost::asio::buffer(packetString.str(),packetString.str().size()));
				//std::cout << packetString.str() << endl;

				// clear packet string
				packetString.str(std::string());
				packetString.clear();
			}

			if(packetString.str().size() > 0){
				packetString << std::endl;
			}

			packetString << cmdString;
		}else{ // We haven't nothing to send. Sleep thread for some time.
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
		}
	}

	// end thread
}

void XCI_Parrot::receiveNavData(){
  uint8_t message[NAVDATA_MAX_SIZE];
  navdata_t* navdata = (navdata_t*) &message[0];
  int32_t flag = 1; // 1 - unicast, 2 - multicast
  socketData->send(boost::asio::buffer((uint8_t*)(&flag),sizeof(int32_t)));
  
	size_t receiveSize = 0;
  receiveSize = socketData->receive(boost::asio::buffer(message,NAVDATA_MAX_SIZE));
  sequenceNumberData = navdata->sequence;
  // test god type of navdata
  if(navdata->header == NAVDATA_HEADER){
		if(navdata->options->tag == 0){
			_navdata_demo_t* test = (_navdata_demo_t*) &navdata->options[0];
			printf("Navdata demo altitude: %f \n", test->altitude);
		}

    if(getMaskFromState(navdata->ardrone_state, def_ardrone_state_mask_t::ARDRONE_NAVDATA_BOOTSTRAP)){
			atCommandQueue.push(new atCommandCONFIG("general:navdata_demo","TRUE"));
			std::this_thread::sleep_for(std::chrono::milliseconds(100));

			receiveSize = socketData->receive(boost::asio::buffer(message,NAVDATA_MAX_SIZE));
      sequenceNumberData = navdata->sequence;

			receiveSize = socketData->receive(boost::asio::buffer(message,NAVDATA_MAX_SIZE));
      bool bootstrap = getMaskFromState(navdata->ardrone_state,ARDRONE_NAVDATA_BOOTSTRAP);
      sequenceNumberData = navdata->sequence;
    }
  }

  while(!endAll){
    
  }
}

void XCI_Parrot::receiveVideo(){
	while(!endAll){
		
	}
}

// ----------------- Public function ---------------- //

void XCI_Parrot::init(){
	sequenceNumberCMD = 1;
	sequenceNumberVideo = 1;
	sequenceNumberData = 1;

	endAll = false;

  initNetwork();
	sendingATCmdThread = std::move(std::thread(&XCI_Parrot::sendingATCommands,this));
  receiveNavDataThread = std::move(std::thread(&XCI_Parrot::receiveNavData, this));
	receiveVideoThread = std::move(std::thread(&XCI_Parrot::receiveVideo, this));
	
  atCommandQueue.push(new atCommandCOMWDG());
  atCommandQueue.push(new atCommandFTRIM());
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

  /*
  this_thread::sleep_for(std::chrono::microseconds(1000000));
  
	for(int i=0;i<25;++i){
		parrot.sendCommand("TakeOff");
		this_thread::sleep_for(std::chrono::microseconds(100000));
	}

  this_thread::sleep_for(std::chrono::microseconds(2000000));

	for(int i=0;i<20;++i){
		parrot.sendFlyParam(0,0,0.2,0);
		this_thread::sleep_for(std::chrono::microseconds(200000));
	}

	for(int i=0;i<20;++i){
		parrot.sendFlyParam(0,0,-0.2,0);
		this_thread::sleep_for(std::chrono::microseconds(200000));
	}

	for(int i=0;i<10;++i){
		parrot.sendCommand("Land");
		this_thread::sleep_for(std::chrono::microseconds(100000));
	}

  this_thread::sleep_for(std::chrono::microseconds(5000000));
  */
  while(true){
  };
}

