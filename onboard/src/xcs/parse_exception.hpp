/* 
 * File:   ParseException.hpp
 * Author: michal
 *
 * Created on January 26, 2014, 7:11 PM
 */

#ifndef PARSEEXCEPTION_HPP
#define	PARSEEXCEPTION_HPP

#include "exception.hpp"
#include "xcs/xcs_export.h"

namespace xcs {

    class XCS_EXPORT ParseException : public Exception {
    public:
        ParseException();
        ParseException(const std::string& message);
        virtual ~ParseException();
    private:

    };
};

#endif	/* PARSEEXCEPTION_HPP */

