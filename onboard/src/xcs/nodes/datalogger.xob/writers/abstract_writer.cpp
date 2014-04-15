#include "abstract_writer.hpp"

using namespace std;
using namespace std::chrono;
using namespace xcs::nodes::datalogger;

void AbstractWriter::init(const std::string &dataName, const TimePoint startTime, std::ofstream* file, std::mutex *lock, ::urbi::UVar &uvar) {
    startTime_ = startTime;
    file_ = file;
    dataName_ = dataName;
    lock_ = lock;
    uvar_ = &uvar;
}

void AbstractWriter::writeRecordBegin() {
    auto time = duration_cast<milliseconds>(highResolutionClock_.now() - startTime_).count();

    *file_ << time << "\t" << dataName_ << "\t";
}
