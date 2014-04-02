#include "general_writer.hpp"

using namespace std;
using namespace std::chrono;
using namespace xcs::nodes::datalogger;

GeneralWriter::GeneralWriter(const std::string &name) :
  AbstractWriter(name) {
}

void GeneralWriter::init(const std::string &dataName, const TimePoint startTime, std::ofstream* file, std::mutex *lock, ::urbi::UVar &uvar) {
    basicInit(dataName, startTime, file, lock, uvar);
    UNotifyChange(uvar, &GeneralWriter::write);
}

void GeneralWriter::write(urbi::UVar &uvar) {
    std::lock_guard<std::mutex> lck(*lock_);

    writeRecordBegin();
    *file_ << uvar.val() << endl;
}
