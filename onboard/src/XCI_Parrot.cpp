#include "XCI_Parrot.hpp"

using namespace boost::asio::ip;

// ----------------- Private function --------------- //
const std::string XCI_Parrot::name ="Parrot AR Drone 2.0 XCI";

// ----------------- Public function ---------------- //

void XCI_Parrot::init(){
	boost::asio::io_service io_service;
	parrotCMD = udp::endpoint(address::from_string("192.168.1.1"),CMDPort);
}

std::string XCI_Parrot::getName(){
	return name;
}

senzorList XCI_Parrot::getSenzorList(){
	return senzorList();
}

void* XCI_Parrot::getSenzorData(Senzor senzor){
	return NULL;
}

std::string XCI_Parrot::getConfiguration(){
	return "";
}

specialCMDList XCI_Parrot::getSpecialCMD(){
	specialCMDList CMDList;
	CMDList.push_back("TakeOff");
	CMDList.push_back("Land");
	CMDList.push_back("EmergencyStop");
	CMDList.push_back("Reset");
	return CMDList;
}
	
void XCI_Parrot::setConfiguration(std::string configuration){

}

void XCI_Parrot::sendCommand(std::string command){

}

void XCI_Parrot::sendFlyParam(double roll,double pitch, double yaw, double gaz){

}

void main(){
	XCI_Parrot parrot;
	parrot.init();
}