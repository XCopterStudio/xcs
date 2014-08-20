#ifndef ATCOMMAND_SENDER_H
#define ATCOMMAND_SENDER_H

#include <atomic>
#include <sstream>
#include <string>

#include <xcs/tsqueue.hpp>
#include "at_command.hpp"

#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>

namespace xcs{
namespace xci{
namespace parrot{

    typedef xcs::Tsqueue<AtCommandPtr> AtCommandQueue;

    /*! \brief Maintains link with AR.Drone 2.0 AtCommand channel and send to it all AtCommands from atCommandQueue.
    
        Use asynchronous socket with reconnection when connection is lost.
    */
    class AtCommandSender{
        static const unsigned int TIMEOUT;
        static const int32_t DEFAULT_SEQUENCE_NUMBER;
        static const unsigned int AT_CMD_PACKET_SIZE;

        boost::asio::deadline_timer deadline_;
        boost::asio::deadline_timer wait_;
        boost::asio::ip::udp::socket socket_;
        boost::asio::ip::udp::endpoint parrot_;

        AtCommandQueue& atCommandQueue_;
        std::string lastAtcommand_;

        uint32_t sequenceNumber_;

        volatile std::atomic<bool> end_;

        void handleConnectedAtCommand(const boost::system::error_code& ec);
        void sendAtCommand();
        void handleWritedAtCommand(const boost::system::error_code& ec);
        void checkDeadlineAtCommand();

    public:
        /*! Initialize data structure.
        
            \param atCommandQueue reference on AtCommandQueue of which take an AtCommand and send it to the AR.Drone 2.0
            \param io_service AtCommandSender use this boost io_service for network communication
            \param ipAddress ip address of the AR.Drone 2.0 with whom maintain connection
            \param port is AtCommand port on the AR.Drone 2.0
        */
        AtCommandSender(AtCommandQueue& atCommandQueue, boost::asio::io_service& io_service, std::string ipAddress = "192.168.1.1", unsigned int port = 5556);
        /*! End all operation in progress and close connection with the AR.Drone 2.0. */
        ~AtCommandSender();
        /*! Create connection with the AR.Drone 2.0*/
        void connect();
    };

}}}
#endif
