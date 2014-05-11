#include "general_writer.hpp"

using namespace std;
using namespace std::chrono;
using namespace xcs::nodes::datalogger;

GeneralWriter::GeneralWriter(const std::string &name) :
  AbstractWriter(name) {
}

void GeneralWriter::init(const std::string &dataName, LoggerContext &context, ::urbi::UVar &uvar) {
    basicInit(dataName, context, uvar);
    UNotifyChange(uvar, &GeneralWriter::write);
}

void GeneralWriter::write(urbi::UVar &uvar) {
    if (!context_->enabled) {
        return;
    }
    
    std::lock_guard<std::mutex> lck(context_->lock);

    writeRecordBegin();
    context_->file << uvar.val() << endl;
}
