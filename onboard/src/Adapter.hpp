/* 
 * File:   Adapter.hpp
 * Author: michal
 *
 * Created on January 21, 2014, 8:46 PM
 */

#ifndef ADAPTER_HPP
#define	ADAPTER_HPP

#include "Forest.hpp"
#include <boost/asio.hpp>
#include <string>

class Adapter {
public:
    
    
    Adapter(Forest & forest);

    void start(const std::string & host, const int port);

    virtual ~Adapter();

private:
    const int BUFFER_SIZE = 1024;
    Forest & forest;
    std::string host;
    
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::socket *socketServer;

};

#endif	/* ADAPTER_HPP */

