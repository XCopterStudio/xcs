#include "vector_writer.hpp"

using namespace std;
using namespace std::chrono;
using namespace xcs::nodes::datalogger;

VectorWriter::VectorWriter(const std::string &name) :
  AbstractWriter(name) {
}

void VectorWriter::init(const std::string &syntacticType, const std::string &dataName, const TimePoint startTime, std::ofstream* file, std::mutex *lock, ::urbi::UVar &uvar) {
    basicInit(dataName, startTime, file, lock, uvar);

#define DECLARE(Type) if(syntacticType == #Type) UNotifyChange(uvar, &VectorWriter::write<Type>);
    LIBPORT_LIST_APPLY(DECLARE, VECTOR_TYPES)
}

