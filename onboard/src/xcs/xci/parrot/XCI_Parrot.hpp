#ifndef XCI_PARROT_H
#define XCI_PARROT_H

#include <boost/asio.hpp>
#include <thread>
#include <vector>
#include <atomic>

#include "xcs/xci/XCI.hpp"
#include "AT_Command.hpp"
#include "xcs/tsqueue.hpp"
#include "ardrone_state.hpp"
#include "navdata_options.hpp"
#include "navdata_parser.hpp"
#include "xcs/xci/ConnectionErrorException.hpp"

namespace xcs{
namespace xci{
namespace parrot{

    class XCI_Parrot : public virtual XCI {
        // Constant
        static const int PORT_COM;
        static const int PORT_CMD;
        static const int PORT_VIDEO;
        static const int PORT_DATA;

        static const float EPSILON;

        static const unsigned int AT_CMD_PACKET_SIZE;

        static const std::string NAME;

        static const int32_t DEFAULT_SEQUENCE_NUMBER;

        static const unsigned int VIDEO_MAX_SIZE;

        // Sequence number for communication with the drone.
        uint32_t sequenceNumberCMD_;
        uint32_t sequenceNumberData_;

        // queue for at commands
        Tsqueue<AtCommand*> atCommandQueue_;

        // actual state of ar.drone 2.0
        ArdroneState state_;

        //
        DataReceiver dataSender_;

        // threads
        std::thread threadSendingATCmd_;
        std::thread threadReceiveNavData_;
        std::thread threadReceiveVideo_;

        // end all thread
        volatile std::atomic<bool> endAll_;

        boost::asio::io_service io_serviceCMD_;
        boost::asio::io_service io_serviceData_;
        boost::asio::io_service io_serviceVideo_;

        boost::asio::ip::udp::socket *socketCMD_;
        boost::asio::ip::udp::socket *socketData_;
        boost::asio::ip::tcp::socket *socketVideo_;

        void initNetwork();
        void sendingATCommands();
        void receiveNavData();
        void receiveVideo();

        // function for navdata handling
        void initNavdataReceive();
        void processState(uint32_t droneState);
        void processNavdata(std::vector<NavdataOption*> &options);
        NavdataOption* getOption(NavdataOption* ptr, NavdataTag tag);
        std::string downloadConfiguration() throw (ConnectionErrorException);

    public:
        XCI_Parrot(const DataReceiver dataReceiver) : XCI(dataReceiver) {};
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
}}}
#endif