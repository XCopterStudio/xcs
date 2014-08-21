#ifndef XCI_PARROT_H
#define XCI_PARROT_H


#include <xcs/xci/xci.hpp>
#include <xcs/tsqueue.hpp>
#include <xcs/xci/connection_error_exception.hpp>

#include "at_command.hpp"
#include "navdata_receiver.hpp"
#include "ardrone_state.hpp"
#include "video_decode.hpp"
#include "video_receive.hpp"
#include "at_command_sender.hpp"
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

typedef xcs::Tsqueue<AtCommandPtr> AtCommandQueue;

/*! Xci implementation for Parrot AR.Drone 2.0 */
class XciParrot : public virtual Xci {
    // Constant
    static const float EPSILON;

    //! Name of xci
    static const std::string NAME; 

    //! Queue for storing atcommand which atcommand sender will send to the parrot
    AtCommandQueue atCommandQueue_; 

    //! Resources (thread, connections) initialized
    std::atomic<bool> inited_;

    // actual state of ar.drone 2.0
    ArdroneState state_;

    //! Decode received h264 video from parrot 2.0
    VideoDecoder videoDecoder_;

    //! Threads
    std::thread threadSendingATCmd_; //!< Handler on atcommand sender thread
    std::thread threadReceiveNavData_; //!< Handler on navdata receiver thread
    std::thread threadReadVideoReceiver_; //!< Handler on video receiver thread
    std::thread threadReadVideoData_; //!< Handler on decode video thread
    std::thread threadConfiguration_; //!< Handler on configuration receiver thread

    //! Indicate closing of the xci_parrot -> threads use for its end
    std::atomic<bool> endAll_;

    //! Store all configuration information from parrot 2.0 and xci_parrot
    InformationMap configuration_;

    //! Boost io services for network 
    boost::asio::io_service io_serviceCMD_;
    boost::asio::io_service io_serviceNavdata_;
    boost::asio::io_service io_serviceVideo_;
    boost::asio::io_service io_serviceConfiguration_;

    //! Send atCommand from atCommand queue to the parrot 2.0
    AtCommandSender atCommandSender_;
    //! Receive encoded video from parrot 2.0
    VideoReceiver videoReceiver_;
    //! Receive navdata from parrot 2.0
    NavdataReceiver navdataReceiver_;
    //! Receive configuration from parrot 2.0
    ConfigurationReceiver configurationReceiver_;

    //! Start all network connection and all network threads
    void initNetwork();
    //! Get video frame from receiver, decode it and call dataReceiver_.notify 
    void processVideoData();

    //! Try send confirmed configuration in to a parrot 2.0 
    bool setConfirmedConfigure(AtCommandPtr command);
    //! Try send default configuration in to a parrot 2.0
    bool setDefaultConfiguration();

public:
    /*! Initialize private variables

        \param dataReceiver serve for publishing new sensor data
        \ipAddress of the AR.Drone 2.0 with whom xci_parrot will maintain connection
    */
    XciParrot(DataReceiver &dataReceiver, std::string ipAddress = "192.168.1.1");
    //! Wait until all threads ends
    ~XciParrot();

    //! Initialize Xci for use. Connect to the AR.Drone 2.0.
    void init();

    //! Stop producing data
    void stop();
    //! Return name of x-copter Xci
    std::string name();
    //!Return list of all available sensors on the AR.Drone 2.0
    SensorList sensorList();
    //! Return x-copter�s configuration
    /*!
        \param key name of an AR.Drone 2.0 or xci configuration field
        \return value of desired key or empty string
    */
    std::string configuration(const std::string &key);
    //! Return x-copter�s configuration
    /*!
        \return All available configuration fields with values as map of pairs <name,value>
    */
    InformationMap configuration();
    //! Return list of x-copter�s special commands 
    /*!
        \sa command()
        \return Names of all special command which user can invoke on AR.Drone 2.0 as TakeOff, Land etc.
    */
    SpecialCMDList specialCMD();

    //! Take new x-copter�s configuration and send this configuration to the x-copter
    /*!
        \param key name of configuration field according to the documentation
        \param value desire value which xci try set on parrot 2.0
    */
    void configuration(const std::string &key, const std::string &value);
    //! Take new x-copter�s configuration and send this configuration to the x-copter
    /*!
        \param configuration map of pairs <name,value> which xci try set on parrot 2.0
    */
    void configuration(const InformationMap &configuration);
    //! Take command from list of x-copter�s special commands and send it to the x-copter
    /*!
        \sa specialCMD()
        \param command parrot 2.0 special command(TakeOff, Land etc.) or fly sequence as string name
    */
    void command(const std::string &command);
    //! Take four values for controlling drone 2.0 roll, pitch, yaw and gaz
    /*!
        All values have to be in range <-1,1> or It will be trimmed on this range. Parrot 2.0 mapped this range on <-max angle, max angle>.
        \param roll tilt on sides, -1 mean left, 1 right
        \param pitch nose tilt, -1 mean forward, 1 backward
        \param yaw rotation velocity, -1 mean nose rotation on left side, 1 mean nose rotation on right side
        \param gaz rise up or ascension speed, -1 mean max ascension speed, 1 mean max rise up speed
    */
    void flyControl(float roll, float pitch, float yaw, float gaz);
};
}
}
}
#endif