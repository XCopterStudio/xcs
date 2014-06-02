#include "configuration_receiver.hpp"

#include <xcs/logging.hpp>
#include <thread>

#include <boost/bind.hpp>

using namespace xcs::xci;
using namespace xcs::xci::parrot;
using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;

const unsigned int ConfigurationReceiver::TIMEOUT = 100;// ms

void ConfigurationReceiver::handleConnectedConfiguration(const boost::system::error_code& ec){
    if (end_) {
        return;
    }

    if (!socket_.is_open()) {
        XCS_LOG_WARN("Connect configuration socket timed out.");
        connect();
    }
    else if (ec){
        XCS_LOG_WARN("Connect configuration socket error : " + ec.message());
        socket_.close();
        connect();
    }
    else{
        XCS_LOG_INFO("Configuration receiver connected.");
        receiveConfiguration();
    }
};

void ConfigurationReceiver::receiveConfiguration(){
    if (end_) {
        return;
    }

    deadline_.expires_at(boost::posix_time::pos_infin);
    socket_.async_receive(boost::asio::buffer(buffer_, BUFFER_SIZE), boost::bind(&ConfigurationReceiver::handleReceivedConfiguration, this, _1, _2));
};

void ConfigurationReceiver::handleReceivedConfiguration(const boost::system::error_code& ec, std::size_t bytes_transferred){
    if (end_) {
        return;
    }

    if (ec) {
        XCS_LOG_WARN("Configuration receive error : " + ec.message());
        socket_.close();
        connect();
    }
    else{
        
        cout << buffer_ << endl;
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

        // There is no longer an active deadline. The expiry is set to positive
        // infinity so that the actor takes no action until a new deadline is set.
        deadline_.expires_at(boost::posix_time::pos_infin);
    }
    else{
        deadline_.async_wait(boost::bind(&ConfigurationReceiver::checkDeadlineConfiguration, this));
    }
};

// ======================= public functions ===============================

ConfigurationReceiver::ConfigurationReceiver(AtCommandQueue& atCommandQueue, InformationMap& configuration, boost::asio::io_service& io_service, std::string ipAdress, unsigned int port)
: deadline_(io_service),
socket_(io_service),
parrot_(address::from_string(ipAdress), port),
atCommandQueue_(atCommandQueue),
configuration_(configuration)
{
    end_ = false;
    update_ = false;
};

ConfigurationReceiver::~ConfigurationReceiver(){
    end_ = true;
    socket_.close();
};

void ConfigurationReceiver::connect(){
    if (end_){
        return;
    }

    socket_.open(tcp::v4());

    XCS_LOG_INFO("Try connect configuration socket.");

    deadline_.expires_from_now(boost::posix_time::milliseconds(TIMEOUT));
    socket_.async_connect(parrot_,
        boost::bind(&ConfigurationReceiver::handleConnectedConfiguration, this, _1));

    deadline_.async_wait(boost::bind(&ConfigurationReceiver::checkDeadlineConfiguration, this));
};

void ConfigurationReceiver::update(){
    atCommandQueue_.push(new AtCommandCTRL(STATE_CFG_GET_CONTROL_MODE));
}