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
#include "video_decode.hpp"
#include "xci_exception.hpp"

namespace xci_parrot {

    class XCI_Parrot : public virtual XCI {
        // Constant
        static const int PORT_COM;
        static const int PORT_CMD;
        static const int PORT_VIDEO;
        static const int PORT_DATA;
        
        static const float EPSILON;

        static const unsigned int AT_CMD_PACKET_SIZE;

        static const std::string NAME;

        static const int DEFAULT_SEQUENCE_NUMBER;

        static const unsigned int VIDEO_MAX_SIZE;

        // Sequence number for communication with the drone.
        unsigned int sequenceNumberCMD;
        unsigned int sequenceNumberData;

        // queue for at commands
        tsqueue<atCommand*> atCommandQueue;

        // actual state of ar.drone 2.0
        ardroneState state;
        // video decoder
        VideoDecoder videoDecoder;

        // threads
        std::thread sendingATCmdThread;
        std::thread receiveNavDataThread;
        std::thread receiveVideoThread;

        // end all thread
        volatile std::atomic<bool> endAll;

        boost::asio::io_service io_serviceCMD;
        boost::asio::io_service io_serviceData;
        boost::asio::io_service io_serviceVideo;

        boost::asio::ip::udp::socket *socketCMD;
        boost::asio::ip::udp::socket *socketData;
        boost::asio::ip::tcp::socket *socketVideo;

        void initNetwork();
        void sendingATCommands();
        void receiveNavData();
        void receiveVideo();

        // function for navdata handling
        void initNavdataReceive();
        bool isCorrectData(navdata_t* navdata, const size_t size);
        void processReceivedNavData(navdata_t* navdata, const size_t size);
        navdata_option_t* getOption(navdata_option_t* ptr, navdata_tag_t tag);
        std::string downloadConfiguration() throw (ConnectionErrorException);

    public:
        //! Initialize XCI for use
        void init() throw (ConnectionErrorException);
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
        //! Return x-copter�s configuration
        std::string getConfiguration(const std::string &key);
        //! Return x-copter�s configuration
        informationMap getConfiguration();
        //! Return list of x-copter�s special commands 
        specialCMDList getSpecialCMD();

        //! Take new x-copter�s configuration and send this configuration to the x-copter
        int setConfiguration(const std::string &key, const std::string &value);
        //! Take new x-copter�s configuration and send this configuration to the x-copter
        int setConfiguration(const informationMap &configuration);
        //! Take command from list of x-copter�s special commands and send it to the x-copter
        void sendCommand(const std::string &command);
        //! Take four fly parameters and send it to the x-copter
        void sendFlyParam(float roll, float pitch, float yaw, float gaz);

        ~XCI_Parrot();
    };

}
#endif