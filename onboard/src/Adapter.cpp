/* 
 * File:   Adapter.cpp
 * Author: michal
 * 
 * Created on January 21, 2014, 8:46 PM
 */

#include "Adapter.hpp"
#include <boost/system/error_code.hpp>
#include <vector>


using namespace boost::asio::ip;
using namespace std;

Adapter::Adapter(Forest & forest) : forest(forest) {

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
    this->socketServer = new tcp::socket(this->io_service);
    this->socketServer->connect(serverPoint);

    int readBytes = 0;
    cerr << "Start receiving data..." << endl;

    try {
        while (readBytes = this->socketServer->read_some(bufferSequence)) {
            string command = string(buffer, buffer + readBytes);
            this->forest.doCommand(command);
        }
        cerr << "Error occured during read." << endl;
    } catch (exception &e) {
        cerr << "Error: " << e.what() << endl;
    }
    
    delete this->socketServer;
    delete[] buffer;
}



