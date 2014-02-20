/* 
 * File:   Exception.hpp
 * Author: michal
 *
 * Created on January 26, 2014, 7:02 PM
 */

#ifndef EXCEPTION_HPP
#define	EXCEPTION_HPP

#include <stdexcept>
#include <string>
#include "xcs/xcs_export.h"

namespace xcs {

    class XCS_EXPORT Exception : public std::runtime_error {
    public:
        Exception();
        Exception(const std::string & message);
        virtual ~Exception();
    };
};

#endif	/* EXCEPTION_HPP */

