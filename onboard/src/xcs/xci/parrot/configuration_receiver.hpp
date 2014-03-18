#ifndef CONFIGURATION_RECEIVER_H
#define CONFIGURATION_RECEIVER_H

#include <atomic>
#include <string>
#include <sstream>
#include <iostream>

#include <xcs/tsqueue.hpp>
#include "AT_Command.hpp"

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

        std::stringstream configuration_;

        char buffer[BUFFER_SIZE];

        volatile std::atomic<bool> connected_;
        volatile std::atomic<bool> received_;
        volatile std::atomic<bool> receivedTimeOut_;
        volatile std::atomic<bool> end_;

        void connect();

        void handleConnectedConfiguration(const boost::system::error_code& ec);
        void receiveConfiguration();
        void handleReceivedConfiguration(const boost::system::error_code& ec, std::size_t bytes_transferred);
        void checkDeadlineConfiguration();
    public:
        ConfigurationReceiver(AtCommandQueue& atCommandQueue, boost::asio::io_service& io_service, std::string ipAdress = "192.168.1.1", unsigned int port = 5554);
        ~ConfigurationReceiver();
        std::string getConfiguration();
    };

}}}
#endif