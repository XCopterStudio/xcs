#include "vector_writer.hpp"

using namespace std;
using namespace std::chrono;
using namespace xcs::nodes::datalogger;
using namespace xcs::nodes::xci;

VectorWriter::VectorWriter(const std::string &name) :
  AbstractWriter(name) {
}

void VectorWriter::init(const std::string &dataName, const TimePoint startTime, std::ofstream* file, std::mutex *lock, ::urbi::UVar &uvar) {
    basicInit(dataName, startTime, file, lock, uvar);
    UNotifyChange(uvar, &VectorWriter::write);
}

void VectorWriter::write(CartesianVectorChronologic value) {
    std::lock_guard<std::mutex> lck(*lock_);

    writeRecordBegin();
    *file_ << value.x << "\t" << value.y << "\t" << value.z << "\t" << value.time << endl;
}

//void VectorWriter::write(EulerianVectorChronologic value) {
//    std::lock_guard<std::mutex> lck(*lock_);
//
//    writeRecordBegin();
//    *file_ << value.phi << "\t" << value.theta << "\t" << value.psi << "\t" << value.time << endl;
//}

//void VectorWriter::write(std::string value) {
//    std::lock_guard<std::mutex> lck(*lock_);
//
//    writeRecordBegin();
//    *file_ << value << endl;
//}
//
//void VectorWriter::write(::urbi::UDictionary value) {
//    std::lock_guard<std::mutex> lck(*lock_);
//
//    writeRecordBegin();
//    *file_ << value << endl;
//}
