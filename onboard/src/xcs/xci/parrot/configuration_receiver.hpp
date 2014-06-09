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

    typedef xcs::Tsqueue< AtCommand* > AtCommandQueue;

    const unsigned int BUFFER_SIZE = 1024;

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
        ConfigurationReceiver(AtCommandQueue& atCommandQueue, InformationMap& configuration, boost::asio::io_service& io_service, std::string ipAdress = "192.168.1.1", unsigned int port = 5559);
        ~ConfigurationReceiver();
        void connect();
        void update();
    };

}}}
#endif
