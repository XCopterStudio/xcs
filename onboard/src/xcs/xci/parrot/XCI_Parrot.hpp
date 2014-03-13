#ifndef XCI_PARROT_H
#define XCI_PARROT_H

#include "navdata_receiver.hpp"
#include <xcs/xci/xci.hpp>
#include "AT_Command.hpp"
#include <xcs/tsqueue.hpp>
#include "ardrone_state.hpp"
#include <xcs/xci/connection_error_exception.hpp>
#include "video_decode.hpp"
#include "video_receive.hpp"
#include "atcommand_sender.hpp"

#include <thread>
#include <vector>
#include <atomic>

#include <boost/asio.hpp>

namespace xcs {
namespace xci {
namespace parrot {

enum ParrotFrameType {
    FRAME_TYPE_UNKNNOWN = 0,
    FRAME_TYPE_IDR_FRAME, /* headers followed by I-frame */
    FRAME_TYPE_I_FRAME,
    FRAME_TYPE_P_FRAME,
    FRAME_TYPE_HEADERS
};

typedef xcs::Tsqueue< AtCommand* > AtCommandQueue;

class XCI_Parrot : public virtual XCI {
    // Constant
    static const int PORT_COM;
    static const int PORT_CMD;
    static const int PORT_VIDEO;
    static const int PORT_DATA;

    static const float EPSILON;

    static const std::string NAME;

    static const unsigned int VIDEO_MAX_SIZE;

    // queue for at commands
    AtCommandQueue atCommandQueue_;

    // actual state of ar.drone 2.0
    ArdroneState state_;

    //video decoder 
    VideoDecoder videoDecoder_;

    // threads
    std::thread threadSendingATCmd_;
    std::thread threadReceiveNavData_;
	std::thread threadReadVideoReceiver_;
    std::thread threadReadVideoData_;

    // end all thread
    volatile std::atomic<bool> endAll_;

    boost::asio::io_service io_serviceCMD_;
    boost::asio::io_service io_serviceNavdata_;
	boost::asio::io_service io_serviceVideo_;

    AtCommandSender atCommandSender_;
    VideoReceiver videoReceiver_;
    NavdataReceiver navdataReceiver_;

    void initNetwork();
    void sendingATCommands();

    void processVideoData();

    std::string downloadConfiguration() throw (ConnectionErrorException);

public:

    XCI_Parrot(DataReceiver &dataReceiver, std::string ipAddress = "192.168.1.1")
        : XCI(dataReceiver),
        atCommandSender_(atCommandQueue_, io_serviceCMD_ ,ipAddress, PORT_CMD),
        videoReceiver_(io_serviceVideo_, ipAddress, PORT_VIDEO),
        navdataReceiver_(dataReceiver, atCommandQueue_, state_, io_serviceNavdata_, ipAddress, PORT_DATA)
      {
    };
    //! Initialize XCI for use
    void init() throw (ConnectionErrorException);
    //! Resets settings to default values and re-calibrates the sensors (if supported).
    void reset();
    //! Turns on the engines.
    void start();
    //! Turns off the engines.
    void stop();
    //! Return name of x-copter XCI
    std::string name();
    //!Return list of available sensor on x-copter
    SensorList sensorList();
    //! A pure virtual member returning specific x-copter's parameter.
    ParameterValueType parameter(ParameterNameType name);
    //! Take specification of sensor and return void pointer to data from desired sensor
    void* sensorData(const Sensor &sensor);
    //! Return x-copter�s configuration
    std::string configuration(const std::string &key);
    //! Return x-copter�s configuration
    InformationMap configuration();
    //! Return list of x-copter�s special commands 
    SpecialCMDList specialCMD();

    //! Take new x-copter�s configuration and send this configuration to the x-copter
    void configuration(const std::string &key, const std::string &value);
    //! Take new x-copter�s configuration and send this configuration to the x-copter
    void configuration(const InformationMap &configuration);
    //! Take instance of the DataReceiver
    void dataReceiver(DataReceiver* dataReceiver);
    //! Take command from list of x-copter�s special commands and send it to the x-copter
    void command(const std::string &command);
    //! Take four fly parameters and send it to the x-copter
    void flyParam(float roll, float pitch, float yaw, float gaz);

    ~XCI_Parrot();
};
}
}
}
#endif