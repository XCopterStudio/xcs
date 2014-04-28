#ifndef VectorWriter_HPP
#define	VectorWriter_HPP

#include <string>
#include <urbi/uobject.hh>

#include <xcs/types/cartesian_vector.hpp>
#include <xcs/types/eulerian_vector.hpp>
#include <xcs/types/fly_control.hpp>
#include "abstract_writer.hpp"

#define VECTOR_TYPES LIBPORT_LIST( \
        xcs::EulerianVector, \
        xcs::EulerianVectorChronologic, \
        xcs::CartesianVector, \
        xcs::CartesianVectorChronologic, \
        xcs::FlyControl, )

namespace xcs {
namespace nodes {
namespace datalogger {

class VectorWriter : public AbstractWriter {
public:
    VectorWriter(const std::string &name);
    void init(const std::string &syntacticType, const std::string &dataName, LoggerContext &context, ::urbi::UVar &uvar);

    template<typename T>
    void write(T value) {
        if (!context_->enabled) {
            return;
        }
        
        std::lock_guard<std::mutex> lck(context_->lock);

        writeRecordBegin();

        value.serialize(context_->file);
        context_->file << std::endl;
    }

};


};
}
}


#endif	/* VectorWriter_HPP */

