#ifndef XCI_EXCEPTION_H
#define XCI_EXCEPTION_H

#include <exception>
#include <string>

class ConnectionErrorException : public std::exception{
	const std::string message;

public:
	ConnectionErrorException(const std::string message) : message(message){};

	const char * what () const throw()
  {
		std::string error("Cannot connect to the drone with message: " + message);
		return error.c_str();
  }
};

#endif