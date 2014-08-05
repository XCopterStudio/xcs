#include "scalar_writer.hpp"

using namespace std;
using namespace std::chrono;
using namespace xcs::nodes::datalogger;

ScalarWriter::ScalarWriter(const std::string &name) :
  AbstractWriter(name) {
}

void ScalarWriter::init(const std::string &dataName, LoggerContext &context, ::urbi::UVar &uvar) {
    basicInit(dataName, context, uvar);
    UNotifyChange(uvar, &ScalarWriter::write);
}

void ScalarWriter::write(urbi::UVar &uvar) {
    if (!context_->enabled || !enabled()) {
        return;
    }
    
    std::lock_guard<std::mutex> lck(context_->lock);

    writeRecordBegin();
    context_->file << uvar.val() << endl;
}
