#ifndef XCI_PARROT_H
#define XCI_PARROT_H

#include <boost/asio.hpp>

#include "XCI.hpp"

class XCI_Parrot : public virtual XCI{
	const std::string name;
	const int CMDPort = 5556;
	boost::asio::ip::udp::endpoint parrotCMD;
public:
	//! Inicialize XCI for use
	void init();

	//! Return name of x-copter XCI
	std::string getName();
	//!Return list of available senzor on x-copter
	senzorList getSenzorList();
	//! Take specifikation of senzor and return void pointer to data from desired senzor
	void* getSenzorData(Senzor senzor);
	//! Return x-copter큦 configuration
	std::string getConfiguration();
	//! Return list of x-copter큦 special commands 
	specialCMDList getSpecialCMD();
	
	//! Take new x-copter큦 configuration and send this configuration to the x-copter
	void setConfiguration(std::string configuration);
	//! Take command from list of x-copter큦 special commands and send it to the x-copter
	void sendCommand(std::string command);
	//! Take four fly parameters and send it ot the x-copter
	void sendFlyParam(double roll,double pitch, double yaw, double gaz);
};

#endif