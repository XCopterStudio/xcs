/* 
 * File:   ParseException.cpp
 * Author: michal
 * 
 * Created on January 26, 2014, 7:11 PM
 */

#include "ParseException.hpp"

xcs::ParseException::ParseException() : xcs::Exception() {
}

xcs::ParseException::ParseException(const std::string& message) : xcs::Exception(message) {
}

xcs::ParseException::~ParseException() {
}

