#include "XCI_Parrot.hpp"

#include <array>

#include <boost/thread/thread.hpp>

using namespace std;
using namespace boost::asio::ip;

// ----------------- Private function --------------- //
const std::string XCI_Parrot::name ="Parrot AR Drone 2.0 XCI";
const int XCI_Parrot::CMDPort = 5556;

// ----------------- Public function ---------------- //

void XCI_Parrot::init(){
	sequenceNumber = 1;
	try{
		boost::asio::io_service io_service;
		parrotCMD = udp::endpoint(address::from_string("192.168.1.1"),CMDPort);

		udp::socket socketParrot(io_service);
		socketParrot.open(udp::v4());

		std::array<char,1023> buffer;

		// Take off
		for(int i=0; i < 100; i++){
			// create magic bitField for take off
			int bitField = (1 << 18) | (1 << 20) | (1 << 22) | (1 << 24) | (1 << 28) | (1 << 9);
			int length = sprintf_s(&buffer[0],1024,"AT*REF=%d,%d\r",sequenceNumber++,bitField);
			// send AT-command to x-copter
			socketParrot.send_to(boost::asio::buffer(buffer,length), parrotCMD);
			//print AT-command
			for(int a=0; a<length; a++){
				cout << buffer[a];
			}
		
			boost::this_thread::sleep_for( boost::chrono::milliseconds(50) );
		}

		// Land
		for(int i=0; i < 100; i++){
			// create magic bitField for land
			int bitField = (1 << 18) | (1 << 20) | (1 << 22) | (1 << 24) | (1 << 28);
			int length = sprintf_s(&buffer[0],1024,"AT*REF=%d,%d\r",sequenceNumber++,bitField);
			// send AT-command to x-copter
			socketParrot.send_to(boost::asio::buffer(buffer,length), parrotCMD);
			//print AT-command
			for(int a=0; a<length; a++){
				cout << buffer[a];
			}

			boost::this_thread::sleep_for( boost::chrono::milliseconds(50) );
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