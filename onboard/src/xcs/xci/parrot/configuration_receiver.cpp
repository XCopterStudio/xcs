#include "configuration_receiver.hpp"

#include <thread>

#include <boost/bind.hpp>

using namespace xcs::xci::parrot;
using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;

const unsigned int ConfigurationReceiver::TIMEOUT = 100;// ms

void ConfigurationReceiver::connect(){
    if (end_){
        return;
    }

    // clear all variables
    received_ = false;
    connected_ = false;
    receivedTimeOut_ = false;
    configuration_.clear();
    configuration_.str(string());

    socket_.open(tcp::v4());

    cerr << "Try connect configuration socket." << endl;

    deadline_.expires_from_now(boost::posix_time::milliseconds(TIMEOUT));
    socket_.async_connect(parrot_,
        boost::bind(&ConfigurationReceiver::handleConnectedConfiguration, this, _1));

    deadline_.async_wait(boost::bind(&ConfigurationReceiver::checkDeadlineConfiguration, this));
};

void ConfigurationReceiver::handleConnectedConfiguration(const boost::system::error_code& ec){
    if (end_) {
        return;
    }

    if (!socket_.is_open()) {
        cerr << "Connect configuration socket timed out." << endl;
        //connect();
    }
    else if (ec){
        cerr << "Connect configuration socket error: " << ec.message() << endl;
        //socketNavdata_.close();
        //connect();
    }
    else{
        connected_ = true;
        atCommandQueue_.push(new AtCommandCTRL(STATE_CFG_GET_CONTROL_MODE));
        receiveConfiguration();
    }
};

void ConfigurationReceiver::receiveConfiguration(){
    if (end_) {
        return;
    }

    deadline_.expires_from_now(boost::posix_time::milliseconds(TIMEOUT));
    socket_.async_receive(boost::asio::buffer(buffer, BUFFER_SIZE), boost::bind(&ConfigurationReceiver::handleReceivedConfiguration, this, _1, _2));
};

void ConfigurationReceiver::handleReceivedConfiguration(const boost::system::error_code& ec, std::size_t bytes_transferred){
    if (end_) {
        return;
    }

    if (ec) {
        cerr << "Configuration receive error: " << ec.message() << endl;
        //socketNavdata_.close();
        //connect();
    }
    else{
        received_ = true;
        configuration_.write(buffer, bytes_transferred);
        receiveConfiguration();
    }
};

void ConfigurationReceiver::checkDeadlineConfiguration(){
    if (end_){
        return;
    }

    // Check whether the deadline has passed. We compare the deadline against
    // the current time since a new asynchronous operation may have moved the
    // deadline before this actor had a chance to run.
    if (deadline_.expires_at() <= deadline_timer::traits_type::now()) {
        // The deadline has passed. The socket is closed so that any outstanding
        // asynchronous operations are cancelled.
        socket_.close();

        if (!connected_ || !received_){
            connect();
        }
        else if (received_){
            receivedTimeOut_ = true;
        }

        // There is no longer an active deadline. The expiry is set to positive
        // infinity so that the actor takes no action until a new deadline is set.
        deadline_.expires_at(boost::posix_time::pos_infin);
    }
    else{
        deadline_.async_wait(boost::bind(&ConfigurationReceiver::checkDeadlineConfiguration, this));
    }
};

// ======================= public functions ===============================

ConfigurationReceiver::ConfigurationReceiver(AtCommandQueue& atCommandQueue, boost::asio::io_service& io_service, std::string ipAdress, unsigned int port)
: deadline_(io_service),
socket_(io_service),
parrot_(address::from_string(ipAdress), port),
atCommandQueue_(atCommandQueue)
{
    end_ = false;
    connected_ = false;
    receivedTimeOut_ = false;

    deadline_.expires_at(boost::posix_time::pos_infin);
};

ConfigurationReceiver::~ConfigurationReceiver(){
    end_ = true;
    socket_.close();
};

std::string ConfigurationReceiver::getConfiguration(){
    connect();

    while (!receivedTimeOut_){
        this_thread::sleep_for(chrono::milliseconds(5));
    }

    cerr << configuration_.str() << endl;
    return configuration_.str();
};