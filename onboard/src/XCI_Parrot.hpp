#ifndef XCI_PARROT_H
#define XCI_PARROT_H

#include <boost/asio.hpp>
#include <thread>
#include <atomic>

#include "XCI.hpp"
#include "AT_Command.hpp"
#include "../../shared/tsqueue.hpp"
#include "ardrone_state.hpp"
#include "navdata_common.h"

namespace xci_parrot{

class XCI_Parrot : public virtual XCI{
	// Constant
	static const int CMDPort;
	static const int VideoPort;
	static const int DataPort;

	static const unsigned int atCMDPacketSize;

	static const std::string name;

	static const int defaultSequenceNumber;

	// Sequence number for communication with the drone.
	unsigned int sequenceNumberCMD;
	unsigned int sequenceNumberVideo;
	unsigned int sequenceNumberData;

	// queue for at commands
	tsqueue<atCommand*> atCommandQueue;

	// actual state of ar.drone 2.0
	ardroneState state;

	// threads
	std::thread sendingATCmdThread;
  std::thread receiveNavDataThread;
	std::thread receiveVideoThread;

	// end all thread
	volatile std::atomic<bool> endAll;

  boost::asio::io_service io_service;
	boost::asio::ip::udp::socket *socketCMD;
	boost::asio::ip::udp::socket *socketData;
	boost::asio::ip::tcp::socket *socketVideo;

	void initNetwork();
	void sendingATCommands();
  void receiveNavData();
	void receiveVideo();

	// function for navdata handling
	void initNavdataReceive();
	bool isDataCorrect(navdata_t* navdata, const size_t size);
	void processReceivedNavData(navdata_t* navdata, const size_t size);

public:
	//! Initialize XCI for use
	void init();
	//! Resets settings to default values and re-calibrates the sensors (if supported).
	void reset();
	//! Turns on the engines.
	void start();
	//! Turns off the engines.
	void stop();
	//! Return name of x-copter XCI
	std::string getName();
	//!Return list of available sensor on x-copter
	sensorList getSensorList();
	//! Take specification of sensor and return void pointer to data from desired sensor
	void* getSensorData(const Sensor &sensor);
	//! Return x-copter큦 configuration
	std::string getConfiguration(const std::string &key);
	//! Return x-copter큦 configuration
	informationMap getConfiguration();
	//! Return list of x-copter큦 special commands 
	specialCMDList getSpecialCMD();
	
	//! Take new x-copter큦 configuration and send this configuration to the x-copter
	int setConfiguration(const std::string &key, const std::string &value);
	//! Take new x-copter큦 configuration and send this configuration to the x-copter
	int setConfiguration(const informationMap &configuration);
	//! Take command from list of x-copter큦 special commands and send it to the x-copter
	void sendCommand(const std::string &command);
	//! Take four fly parameters and send it to the x-copter
	void sendFlyParam(float roll, float pitch, float yaw, float gaz);

	~XCI_Parrot();
};

}
#endif