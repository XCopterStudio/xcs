#ifndef CONFIGURATION_RECEIVER_H
#define CONFIGURATION_RECEIVER_H

#include <atomic>
#include <string>
#include <sstream>
#include <iostream>

#include <xcs/tsqueue.hpp>
#include <xcs/xci/xci.hpp>
#include "at_command.hpp"

#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>

namespace xcs{
namespace xci{
namespace parrot{

    typedef xcs::Tsqueue<AtCommandPtr> AtCommandQueue;

    const unsigned int BUFFER_SIZE = 1024;

    //! \brief Maintain connection with the AR.Drone 2.0 configuration channel and update local stored configuration map.
    
        Use asynchronous connection with reconnection when connection is lost.
    */
    class ConfigurationReceiver{
        static const unsigned int TIMEOUT;

        boost::asio::deadline_timer deadline_;
        boost::asio::ip::tcp::socket socket_;
        boost::asio::ip::tcp::endpoint parrot_;

        AtCommandQueue& atCommandQueue_;
        InformationMap& configuration_;

        char buffer_[BUFFER_SIZE];

        volatile std::atomic<bool> end_;

        void parseBuffer();

        void handleConnectedConfiguration(const boost::system::error_code& ec);
        void receiveConfiguration();
        void handleReceivedConfiguration(const boost::system::error_code& ec, std::size_t bytes_transferred);
        void checkDeadlineConfiguration();
    public:
        /*! Initialize data structure.
            
            \param atCommandQueue reference on AtCommand queue in which ConfigurationReceiver will inserting control AtCommands.
            \param configuration reference on configuration map in which ConfigurationReceiver will save configuration when some call update() and after first connection
            \param io_service ConfigurationReceiver use this boost io_service for network communication
            \param ipAddress of the AR.Drone 2.0 with whom maintain connection
            \port is configuration port on the AR.Drone 2.0
        */
        ConfigurationReceiver(AtCommandQueue& atCommandQueue, InformationMap& configuration, boost::asio::io_service& io_service, std::string ipAddress = "192.168.1.1", unsigned int port = 5559);
        /*! End all operation in progress and close connection with the AR.Drone 2.0. */
        ~ConfigurationReceiver();
        /*! Create connection with the AR.Drone 2.0*/
        void connect();
        /*! Send configuration request in the AR.Drone 2.0. Successively update configuration_. */
        void update();
    };

}}}
#endif
