#ifndef VectorWriter_HPP
#define	VectorWriter_HPP

#include <string>
#include <urbi/uobject.hh>

#include <xcs/nodes/xci.xob/structs/cartesian_vector.hpp>
#include <xcs/nodes/xci.xob/structs/eulerian_vector.hpp>
#include "abstract_writer.hpp"

#define VECTOR_TYPES LIBPORT_LIST( \
        xcs::nodes::xci::EulerianVector, \
        xcs::nodes::xci::EulerianVectorChronologic, \
        xcs::nodes::xci::CartesianVector, \
        xcs::nodes::xci::CartesianVectorChronologic, )

namespace xcs {
namespace nodes {
namespace datalogger {

class VectorWriter : public AbstractWriter {
public:
    VectorWriter(const std::string &name);
    void init(const std::string &syntacticType, const std::string &dataName, const TimePoint startTime, std::ofstream* file, std::mutex *lock, ::urbi::UVar &uvar);

    template<typename T>
    void write(T value) {
        std::lock_guard<std::mutex> lck(*lock_);

        writeRecordBegin();

        value.serialize(*file_);
        *file_ << std::endl;
    }

};


};
}
}


#endif	/* VectorWriter_HPP */

