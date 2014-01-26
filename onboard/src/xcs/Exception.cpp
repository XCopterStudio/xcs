/* 
 * File:   Exception.cpp
 * Author: michal
 * 
 * Created on January 26, 2014, 7:02 PM
 */

#include <stdexcept>

#include "Exception.hpp"

xcs::Exception::Exception() : std::runtime_error("XCS Exception") {
}

xcs::Exception::Exception(const std::string & message) : std::runtime_error(message) {
}

xcs::Exception::~Exception() {
}

