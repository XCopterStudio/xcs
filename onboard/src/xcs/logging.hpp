#ifndef LOGGING_H
#define LOGGING_H

#include <boost/log/trivial.hpp>

#ifdef NDEBUG
    #define XCS_LOG_INFO(msg) /* empty */
#else
    #define XCS_LOG_INFO(msg) \
        BOOST_LOG_TRIVIAL(info) << msg   
#endif

#define XCS_LOG_WARN(msg)  \
        BOOST_LOG_TRIVIAL(warning) << msg

#define XCS_LOG_ERROR(msg)  \
        BOOST_LOG_TRIVIAL(error) << msg

#define XCS_LOG_FATAL(msg)  \
        BOOST_LOG_TRIVIAL(fatal) << msg

#endif