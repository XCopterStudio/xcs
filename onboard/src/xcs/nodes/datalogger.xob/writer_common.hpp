#ifndef DATALOGGER_TYPEDEFS_HPP
#define	DATALOGGER_TYPEDEFS_HPP

#include <memory>
#include <list>
#include <chrono>
#include <mutex>
#include <fstream>

namespace xcs {
namespace nodes {
namespace datalogger {
typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::time_point<std::chrono::high_resolution_clock> TimePoint;

}
}
}


#endif	/* DATALOGGER_TYPEDEFS_HPP */

