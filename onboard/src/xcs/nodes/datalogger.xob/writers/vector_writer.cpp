#include "vector_writer.hpp"

using namespace std;
using namespace std::chrono;
using namespace xcs::nodes::datalogger;

VectorWriter::VectorWriter(const std::string &name) :
  AbstractWriter(name) {
}

void VectorWriter::init(const std::string &syntacticType, const std::string &dataName, LoggerContext &context, ::urbi::UVar &uvar) {
    basicInit(dataName, context, uvar);

#define DECLARE(Type) if(syntacticType == #Type) UNotifyChange(uvar, &VectorWriter::write<Type>);
    LIBPORT_LIST_APPLY(DECLARE, VECTOR_TYPES)
}

