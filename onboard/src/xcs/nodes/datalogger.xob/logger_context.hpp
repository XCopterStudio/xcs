#ifndef DATALOGGER_TYPEDEFS_HPP
#define	DATALOGGER_TYPEDEFS_HPP

#include <memory>
#include <list>
#include <chrono>
#include <mutex>
#include <fstream>
#include <atomic>

namespace xcs {
namespace nodes {
namespace datalogger {
typedef std::chrono::high_resolution_clock Clock; // TODO where is it used?
typedef std::chrono::time_point<std::chrono::high_resolution_clock> TimePoint;

struct LoggerContext {

    LoggerContext() :
      startTime(Clock().now()),
      enabled(false) {
    }

    const TimePoint startTime;
    std::mutex lock;
    std::ofstream file;
    std::atomic<bool> enabled;
};
}
}
}


#endif	/* DATALOGGER_TYPEDEFS_HPP */

