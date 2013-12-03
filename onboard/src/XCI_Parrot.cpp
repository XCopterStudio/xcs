#include "XCI_Parrot.hpp"
#include "AT_Command.hpp"

#include <boost/thread/thread.hpp>

using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace xci_parrot;
// ----------------- Constant ----------------------- //

const std::string XCI_Parrot::name ="Parrot AR Drone 2.0 XCI";
const int XCI_Parrot::CMDPort = 5556;
const int XCI_Parrot::VideoPort = 5555;
const int XCI_Parrot::DataPort = 5554;

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

// ----------------- Public function ---------------- //

void XCI_Parrot::init(){
	sequenceNumberCMD = 1;
	sequenceNumberVideo = 1;
	sequenceNumberData = 1;

  initNetwork();

	char buffer_msg[1024];
    try{
		// Take off
		for(int i=0; i < 10; i++){
			// create magic bitField for take off
			int bitField = (1 << 18) | (1 << 20) | (1 << 22) | (1 << 24) | (1 << 28) | (1 << 9);
			int length = sprintf_s(buffer_msg,1024,"AT*REF=%d,%d\r",sequenceNumberCMD++,bitField);
			printf("Buffer %s",&buffer_msg);
			// send AT-command to x-copter
			socketCMD->send(boost::asio::buffer(buffer_msg,length));
		
			boost::this_thread::sleep_for( boost::chrono::milliseconds(20) );
		}

    boost::this_thread::sleep_for( boost::chrono::milliseconds(3500) );

		// Land
		for(int i=0; i < 10; i++){
			// create magic bitField for land
			int bitField = (1 << 18) | (1 << 20) | (1 << 22) | (1 << 24) | (1 << 28);
			int length = sprintf_s(buffer_msg,1024,"AT*REF=%d,%d\r",sequenceNumberCMD++,bitField);
			printf("Buffer %s",&buffer_msg);
			// send AT-command to x-copter
			socketCMD->send(boost::asio::buffer(buffer_msg,length));
			//print AT-command

			boost::this_thread::sleep_for( boost::chrono::milliseconds(20) );
		}
    }catch(exception ex){
      cout << ex.what() << endl;
    }
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
	CMDList.push_back("Reset");
	return CMDList;
}
	
int XCI_Parrot::setConfiguration(const std::string &key, const std::string &value){
	return 1;
}

int XCI_Parrot::setConfiguration(const informationMap &configuration){
	return 1;
}

void XCI_Parrot::sendCommand(const std::string &command){

}

void XCI_Parrot::sendFlyParam(double roll, double pitch, double yaw, double gaz){

}

void main(){
	XCI_Parrot parrot;
	parrot.init();
}