#include "navdata_receiver.hpp"

#include "navdata_parser.hpp"
#include<xcs/logging.hpp>

#include <boost/bind.hpp>

using namespace std;
using namespace xcs::xci;
using namespace xcs::xci::parrot;
using namespace boost::asio;
using namespace boost::asio::ip;

const unsigned int NavdataReceiver::TIMEOUT = 1000; // ms
const int32_t NavdataReceiver::DEFAULT_SEQUENCE_NUMBER = 1;

void NavdataReceiver::handleConnectedNavdata(const boost::system::error_code& ec) {
    if (end_) {
        return;
    }

    if (!socketNavdata_.is_open()) {
        XCS_LOG_WARN("Connect Navdata socket timed out.");
        connect();
    } else if (ec){
        XCS_LOG_WARN("Connect Navdata socket error: " + ec.message());
        socketNavdata_.close();
        connect();
    }else {
        XCS_LOG_INFO("Navdata socket connected.");
        int32_t flag = 1; // 1 - unicast, 2 - multicast
        deadlineNavdata_.expires_from_now(boost::posix_time::milliseconds(TIMEOUT));
        socketNavdata_.async_send(boost::asio::buffer((uint8_t*) (&flag), sizeof (int32_t)), boost::bind(&NavdataReceiver::receiveNavdata, this, _1));
    }
}

void NavdataReceiver::receiveNavdata(const boost::system::error_code& ec) {
    if (end_) {
        return;
    }


    if (!socketNavdata_.is_open()) {
        XCS_LOG_WARN("Navdata receive error connection closed.");
        connect();
    } else if (ec){
        XCS_LOG_WARN("Navdata receive error: " + ec.message());
        socketNavdata_.close();
        connect();
    }else {
        deadlineNavdata_.expires_from_now(boost::posix_time::milliseconds(TIMEOUT));
        socketNavdata_.async_receive(boost::asio::buffer(navdataBuffer, NAVDATA_MAX_SIZE), boost::bind(&NavdataReceiver::handleReceivedNavdata, this, _1, _2));
    }
}

void NavdataReceiver::handleReceivedNavdata(const boost::system::error_code& ec, std::size_t bytes_transferred) {
    if (end_) {
        return;
    }

    if (!socketNavdata_.is_open()){
        XCS_LOG_WARN("Navdata receive closed socket");
        connect();
    }else if (ec) {
        XCS_LOG_WARN("Navdata receive error: " + ec.message());
        socketNavdata_.close();
        connect();
    }else {
        deadlineNavdata_.expires_at(boost::posix_time::pos_infin);

        Navdata* navdata = (Navdata*)& navdataBuffer[0];
        if (navdata->sequence > sequenceNumberNavdata_ && navdata->header == 0x55667788) { // all received data with sequence number lower then sequenceNumberData_ will be skipped.
            uint32_t navdataCks = NavdataProcess::computeChecksum(navdata, bytes_transferred);
            vector<OptionAcceptor*> options = NavdataProcess::parse(navdata, navdataCks, bytes_transferred);
            if (options.size() > 0) {
                processState(navdata->ardrone_state);
                processNavdata(options);
            }

            sequenceNumberNavdata_ = navdata->sequence;
        }

        receiveNavdata(boost::system::error_code());
    }
}


void NavdataReceiver::checkDeadlineNavdata() {
    if (end_){
        return;
    }

    // Check whether the deadline has passed. We compare the deadline against
    // the current time since a new asynchronous operation may have moved the
    // deadline before this actor had a chance to run.
    if (deadlineNavdata_.expires_at() <= deadline_timer::traits_type::now()) {
        // The deadline has passed. The socket is closed so that any outstanding
        // asynchronous operations are cancelled.
        socketNavdata_.close();
        //connect();
        // There is no longer an active deadline. The expiry is set to positive
        // infinity so that the actor takes no action until a new deadline is set.
        deadlineNavdata_.expires_at(boost::posix_time::pos_infin);
    }

    deadlineNavdata_.async_wait(boost::bind(&NavdataReceiver::checkDeadlineNavdata, this));
}

void NavdataReceiver::processState(uint32_t parrotState) {
    parrotState_.updateState(parrotState);

    if (parrotState_.getState(FLAG_ARDRONE_COM_WATCHDOG_MASK)) { // reset sequence number
        sequenceNumberNavdata_ = DEFAULT_SEQUENCE_NUMBER - 1;
        atCommandQueue_.push(std::shared_ptr<AtCommand>(new AtCommandCOMWDG()));
    }

    if (parrotState_.getState(FLAG_ARDRONE_COM_LOST_MASK)) {
        sequenceNumberNavdata_ = DEFAULT_SEQUENCE_NUMBER - 1;
        //initNavdataReceive();
    }

    // parrot is in state in which can not fly
    if (parrotState_.getState(FLAG_ARDRONE_MOTORS_MASK) || parrotState_.getState(FLAG_ARDRONE_SOFTWARE_FAULT) || parrotState_.getState(FLAG_ARDRONE_VBAT_LOW)){
        dataReceiver_.notify("alive", 0);
    }else {
        dataReceiver_.notify("alive", 1);
    }
}

void NavdataReceiver::processNavdata(vector<OptionAcceptor*> &options) {
    OptionVisitor visitor(dataReceiver_);
    for (auto option : options) {
        option->accept(visitor);
        delete option;
    }
}

// ========================== public functions ==================================


NavdataReceiver::NavdataReceiver(DataReceiver& dataReceiver,
    AtCommandQueue& atCommandQueue,
    ArdroneState& parrotState,
    boost::asio::io_service& io_service,
    std::string ipAddress,
    unsigned int port) :
  deadlineNavdata_(io_service),
  socketNavdata_(io_service),
  parrotNavdata_(address::from_string(ipAddress), port),
  dataReceiver_(dataReceiver),
  parrotState_(parrotState),
  atCommandQueue_(atCommandQueue) {
    end_ = false;

    sequenceNumberNavdata_ = DEFAULT_SEQUENCE_NUMBER - 1;

    deadlineNavdata_.expires_at(boost::posix_time::pos_infin);
    deadlineNavdata_.async_wait(boost::bind(&NavdataReceiver::checkDeadlineNavdata, this));
}

NavdataReceiver::~NavdataReceiver() {
    end_ = true;

    socketNavdata_.close();
}

void NavdataReceiver::connect() {
    // connect to navdata port
    if (end_){
        return;
    }

    XCS_LOG_INFO("Try connect navdata receiver.");
    socketNavdata_.open(udp::v4());

    deadlineNavdata_.expires_from_now(boost::posix_time::milliseconds(TIMEOUT));
    socketNavdata_.async_connect(parrotNavdata_,
            boost::bind(&NavdataReceiver::handleConnectedNavdata, this, _1));
};
