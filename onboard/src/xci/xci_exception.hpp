#ifndef XCI_EXCEPTION_H
#define XCI_EXCEPTION_H

#include <exception>
#include <string>

namespace xcs{
namespace xci{
    class ConnectionErrorException : public std::exception {
        const std::string message_;

    public:

        ConnectionErrorException(const std::string message) : message_(message) {
        };

        const char * what() const throw () {
            std::string error("Cannot connect to the drone with message: " + message_);
            return error.c_str();
        }
    };
}}
#endif