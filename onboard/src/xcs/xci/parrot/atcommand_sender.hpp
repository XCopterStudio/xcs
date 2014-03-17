#ifndef ATCOMMAND_SENDER_H
#define ATCOMMAND_SENDER_H

#include <atomic>
#include <sstream>

#include <xcs/tsqueue.hpp>
#include "AT_Command.hpp"

#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>

namespace xcs{
namespace xci{
namespace parrot{

    typedef xcs::Tsqueue< AtCommand* > AtCommandQueue;

    class AtCommandSender{
        static const unsigned int TIMEOUT;
        static const int32_t DEFAULT_SEQUENCE_NUMBER;
        static const unsigned int AT_CMD_PACKET_SIZE;

        boost::asio::deadline_timer deadline_;
        boost::asio::deadline_timer wait_;
        boost::asio::ip::udp::socket socket_;
        boost::asio::ip::udp::endpoint parrot_;

        AtCommandQueue& atCommandQueue_;
        std::stringstream packetString_;

        uint32_t sequenceNumber_;

        volatile std::atomic<bool> end_;

        void handleConnectedAtCommand(const boost::system::error_code& ec);
        void sendAtCommand();
        void handleWritedAtCommand(const boost::system::error_code& ec);
        void checkDeadlineAtCommand();

    public:
        AtCommandSender(AtCommandQueue& atCommandQueue, boost::asio::io_service& io_serviceAtCommand, std::string ipAdress = "192.168.1.1", unsigned int port = 5556);
        ~AtCommandSender();
        void connect();
    };

}}}
#endif