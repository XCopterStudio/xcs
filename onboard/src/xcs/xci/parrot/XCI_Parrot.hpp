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
#include "configuration_receiver.hpp"

#include <thread>
#include <vector>
#include <atomic>
#include <memory>

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
    static const float EPSILON;

    static const std::string NAME;

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

    InformationMap configuration_;

    boost::asio::io_service io_serviceCMD_;
    boost::asio::io_service io_serviceNavdata_;
	boost::asio::io_service io_serviceVideo_;

    AtCommandSender atCommandSender_;
    VideoReceiver videoReceiver_;
    NavdataReceiver navdataReceiver_;
    //ConfigurationReceiver configurationReceiver_;

    void initNetwork();
    void processVideoData();

  
    bool setConfirmedConfigure(AtCommand *command);
    bool setDefaultConfiguration();

public:

    XCI_Parrot(DataReceiver &dataReceiver, std::string ipAddress = "192.168.1.1");
    ~XCI_Parrot();

    //! Initialize XCI for use
    void init();
    //! Return name of x-copter XCI
    std::string name();
    //!Return list of available sensor on x-copter
    SensorList sensorList();
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
    //! Take command from list of x-copter�s special commands and send it to the x-copter
    void command(const std::string &command);
    //! Take four fly controls and send it to the x-copter
    void flyControl(float roll, float pitch, float yaw, float gaz);
};
}
}
}
#endif