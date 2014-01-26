/* 
 * File:   Adapter.hpp
 * Author: michal
 *
 * Created on January 21, 2014, 8:46 PM
 */

#ifndef ADAPTER_HPP
#define	ADAPTER_HPP

#include "Onboard.hpp"
#include <boost/asio.hpp>
#include <string>

class Adapter {
public:


    Adapter(Onboard & forest);

    void start(const std::string & host, const int port);

    virtual ~Adapter();

private:
    const int BUFFER_SIZE = 1024;
    Onboard & onboard;
    std::string host;

    boost::asio::io_service io_service;
    boost::asio::ip::tcp::socket *socketServer;

};

#endif	/* ADAPTER_HPP */

