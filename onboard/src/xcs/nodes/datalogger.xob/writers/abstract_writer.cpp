#include "abstract_writer.hpp"

using namespace std;
using namespace std::chrono;
using namespace xcs::nodes::datalogger;

void AbstractWriter::basicInit(const std::string &dataName, LoggerContext &context, ::urbi::UVar &uvar) {
    context_ = &context;
    dataName_ = dataName;
}

void AbstractWriter::writeRecordBegin() {
    auto time = duration_cast<milliseconds>(highResolutionClock_.now() - context_->startTime).count();

    context_->file << time << "\t" << dataName_ << "\t";
}
