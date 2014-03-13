#include "atcommand_sender.hpp"

#include <string>

#include <boost/bind.hpp>

using namespace std;
using namespace xcs::xci;
using namespace xcs::xci::parrot;
using namespace boost::asio;
using namespace boost::asio::ip;

const unsigned int AtCommandSender::TIMEOUT = 2000; // ms
const int32_t AtCommandSender::DEFAULT_SEQUENCE_NUMBER = 1;
const unsigned int AtCommandSender::AT_CMD_PACKET_SIZE = 1024;

void AtCommandSender::handleConnectedAtCommand(const boost::system::error_code& ec){
    if (end_){
        return;
    }

    if (!socket_.is_open()){
        cerr << "Connect atCommand socket timed out." << endl;
        connect();
    }
    else if (ec){
        cerr << "Connect atCommand socket error: " << ec.message() << endl;
        socket_.close();
        connect();
    }
    else{
        sendAtCommand();
    }
}

void AtCommandSender::sendAtCommand(){
    if (end_){
        return;
    }

    do{
        AtCommand* atCommand = nullptr;
        if (atCommandQueue_.tryPop(atCommand)){
            std::string atCommandString = atCommand->toString(sequenceNumber_++);
            delete atCommand;

            unsigned int packetSize = packetString_.str().size() + atCommandString.size(); // one for new line 
            if (packetSize > AT_CMD_PACKET_SIZE || atCommandQueue_.empty()) { // send prepared packet
                deadline_.expires_from_now(boost::posix_time::milliseconds(TIMEOUT));
                socket_.async_send(boost::asio::buffer(packetString_.str(), packetString_.str().size()), boost::bind(&AtCommandSender::handleWritedAtCommand, this, _1));
                // clear packet string
                packetString_.str(std::string());
                packetString_.clear();
            }

            packetString_ << atCommandString;
            break;
        }
        else{
            wait_.expires_from_now(boost::posix_time::milliseconds(1));
            wait_.async_wait(boost::bind(&AtCommandSender::sendAtCommand, this));
            break;
        }
    } while (true);
}

void AtCommandSender::handleWritedAtCommand(const boost::system::error_code& ec){
    if (end_){
        return;
    }

    if (ec){
        cerr << "Send atCommand data error: " << ec.message() << endl;
        socket_.close();
        connect();
    }
    else{
        sendAtCommand();
    }
}

void AtCommandSender::checkDeadlineAtCommand(){
    if (end_){
        return;
    }

    // Check whether the deadline has passed. We compare the deadline against
    // the current time since a new asynchronous operation may have moved the
    // deadline before this actor had a chance to run.
    if (deadline_.expires_at() <= deadline_timer::traits_type::now())
    {
        // The deadline has passed. The socket is closed so that any outstanding
        // asynchronous operations are cancelled.
        socket_.close();

        // There is no longer an active deadline. The expiry is set to positive
        // infinity so that the actor takes no action until a new deadline is set.
        deadline_.expires_at(boost::posix_time::pos_infin);
        connect();
    }

    deadline_.async_wait(boost::bind(&AtCommandSender::checkDeadlineAtCommand, this));
}

// =========================== public functions ====================

AtCommandSender::AtCommandSender(AtCommandQueue& atCommandQueue, boost::asio::io_service& io_serviceAtCommand, std::string ipAdress, unsigned int port)
: deadline_(io_serviceAtCommand), 
wait_(io_serviceAtCommand), 
socket_(io_serviceAtCommand),
parrot_(address::from_string(ipAdress),port),
atCommandQueue_(atCommandQueue)
{
    end_ = false;

    sequenceNumber_ = DEFAULT_SEQUENCE_NUMBER;

    deadline_.expires_at(boost::posix_time::pos_infin);
    deadline_.async_wait(boost::bind(&AtCommandSender::checkDeadlineAtCommand, this));
}

AtCommandSender::~AtCommandSender(){
    end_ = true;

    socket_.close();
}

void AtCommandSender::connect(){
    if (end_){
        return;
    }

    socket_.open(udp::v4());

    deadline_.expires_from_now(boost::posix_time::milliseconds(TIMEOUT));

    socket_.async_connect(parrot_,
        boost::bind(&AtCommandSender::sendAtCommand, this));
}