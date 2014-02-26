/* 
 * File:   Adapter.cpp
 * Author: michal
 * 
 * Created on January 21, 2014, 8:46 PM
 */

#include "Adapter.hpp"
#include <xcs/parse_exception.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
#include <vector>
#include <iostream>


using namespace boost::asio::ip;
using namespace boost::algorithm;
using namespace boost::system;
using namespace std;
using namespace xcs;

Adapter::Adapter(Onboard & onboard) : onboard_(onboard) {

}

Adapter::~Adapter() {
}

void Adapter::start(const string & host, const int port) {
    // prepare obnoxious Boost buffer
    char *buffer = new char[BUFFER_SIZE];
    boost::asio::mutable_buffer mutableBuffer(buffer, BUFFER_SIZE);
    vector<boost::asio::mutable_buffer> bufferSequence;
    bufferSequence.push_back(mutableBuffer);

    // connect to the server
    tcp::endpoint serverPoint = tcp::endpoint(address::from_string(host), port);
    socketServer_ = new tcp::socket(io_service_);
    socketServer_->connect(serverPoint);

    int readBytes = 0;
    cerr << "Start receiving data..." << endl;

    try {
        //TODO implement a way to stop the adapter
        while (readBytes = socketServer_->read_some(bufferSequence)) {
            string command = string(buffer, buffer + readBytes);
            try {
                parseCommand(command);
            } catch (const ParseException& e) {
                cerr << e.what() << endl;
            }
        }
        cerr << "Error occurred during read." << endl;
    } catch (boost::system::system_error const &e) {
        if (e.code() == boost::asio::error::eof) {
            cerr << "Connection closed by comm server." << endl;
        } else {
            cerr << "Error: " << e.what() << endl;
        }
    }

    delete socketServer_;
    delete[] buffer;
}

void Adapter::parseCommand(const std::string& cmd) {
    string command = cmd;
    trim(command);

    string cmdName = command.substr(0, command.find('('));
    string cmdParams = (command.find('(') != string::npos) ? command.substr(command.find('(') + 1, command.find(')') - 1) : "";

    if (cmdName.size() == 0 && cmdParams.size() > 0) {
        // no command ~ explicit send fly params
        vector<string> params;
        split(params, cmdParams, is_any_of(","));
        if (params.size() != 4) {
            throw xcs::ParseException("Expecting 4 parameters, got '" + cmdParams + "'.");
        }

        onboard_.DoCommand("FlyParam", stod(params[0]), stod(params[1]), stod(params[2]), stod(params[3]));
        //cerr << "[adapter] flyParam " << stod(params[0]) << ", " << stod(params[1]) << ", " << stod(params[2]) << ", " << stod(params[3]) << endl;
    } else if (cmdParams.size() == 0) {
        // no-params command
        onboard_.DoCommand(cmdName);
        //cerr << "[adapter] cmd " << cmdName << endl;
    } else {
        throw xcs::ParseException("Unknown input '" + command + "'.");
    }
}



