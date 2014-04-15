#include "general_writer.hpp"

using namespace std;
using namespace std::chrono;
using namespace xcs::nodes::datalogger;

GeneralWriter::GeneralWriter(const std::string &name) :
  AbstractWriter(name) {
}

void GeneralWriter::start() {
    cerr << "GW::start: " << uvar_ << uvar_->val() << endl;
    
    UNotifyChange(*uvar_, &GeneralWriter::write);
}

void GeneralWriter::write(urbi::UVar &uvar) {
    std::lock_guard<std::mutex> lck(*lock_);

    writeRecordBegin();
    *file_ << uvar.val() << endl;
}
