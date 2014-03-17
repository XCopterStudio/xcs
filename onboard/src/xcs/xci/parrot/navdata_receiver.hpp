#ifndef NAVDATA_RECEIVER_H
#define NAVDATA_RECEIVER_H

#include <atomic>

#include <xcs/tsqueue.hpp>
#include <xcs/xci/xci.hpp>
#include "AT_Command.hpp"
#include "ardrone_state.hpp"
#include "navdata_options.hpp"
#include "options_visitor.hpp"

#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>

namespace xcs{
namespace xci{
namespace parrot{

    typedef xcs::Tsqueue< AtCommand* > AtCommandQueue;

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
        NavdataReceiver(DataReceiver& dataReceiver, AtCommandQueue& atCommandQueue, ArdroneState& parrotState, boost::asio::io_service& io_serviceNavdata, std::string ipAdress = "192.168.1.1", unsigned int port = 5554);
        ~NavdataReceiver();
        void connect();
    };

}}}
#endif