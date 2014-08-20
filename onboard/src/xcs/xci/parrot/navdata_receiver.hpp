#ifndef NAVDATA_RECEIVER_H
#define NAVDATA_RECEIVER_H

#include <atomic>
#include <chrono>

#include <xcs/tsqueue.hpp>
#include <xcs/xci/xci.hpp>
#include "at_command.hpp"
#include "ardrone_state.hpp"
#include "navdata_options.hpp"
#include "options_visitor.hpp"

#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>

namespace xcs{
namespace xci{
namespace parrot{

    typedef xcs::Tsqueue<AtCommandPtr> AtCommandQueue;

    /*! \brief Maintain connection with the AR.Drone 2.0 navdata channel and update sensors outputs according to the received navdata. 

    Use asynchronous connection with reconnection when connection is lost.
    */
    class NavdataReceiver{
        static const unsigned int TIMEOUT;
        static const int32_t DEFAULT_SEQUENCE_NUMBER;

        // navdata buffer
        char navdataBuffer[NAVDATA_MAX_SIZE];

        boost::asio::deadline_timer deadlineNavdata_;
        boost::asio::ip::udp::socket socketNavdata_;
        boost::asio::ip::udp::endpoint parrotNavdata_;

        DataReceiver& dataReceiver_;
        ArdroneState& parrotState_;
        AtCommandQueue& atCommandQueue_;

        uint32_t sequenceNumberNavdata_;

        volatile std::atomic<bool> end_;

        // function for navdata handling
        void handleConnectedNavdata(const boost::system::error_code& ec);
        void receiveNavdata(const boost::system::error_code& ec);
        void handleReceivedNavdata(const boost::system::error_code& ec, std::size_t bytes_transferred);
        void checkDeadlineNavdata();

        void processState(uint32_t droneState);
        void processNavdata(std::vector<OptionAcceptor*> &options);
    public:
        /*! Initialize data structure.

            \param dataReceiver reference on sensor publisher 
            \param atCommandQueue reference on AtCommand queue in which NavdataReceiver will be inserting control AtCommands.
            \param parrotState reference on ArdroneState which will be updated from received navdata packet
            \param io_service NavdataReceiver use this boost io_service for network communication
            \param ipAddress of the AR.Drone 2.0 with whom maintain connection
            \port is navdata port on the AR.Drone 2.0
        */
        NavdataReceiver(DataReceiver& dataReceiver, AtCommandQueue& atCommandQueue, ArdroneState& parrotState, boost::asio::io_service& io_service, std::string ipAddress = "192.168.1.1", unsigned int port = 5554);
        /*! End all operation in progress and close connection with the AR.Drone 2.0. */
        ~NavdataReceiver();
        /*! Create connection with the AR.Drone 2.0*/
        void connect();
    };

}}}
#endif
