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
    
    virtual void init(const std::string &syntacticType, const std::string &dataName, const TimePoint startTime, std::ofstream* file, std::mutex *lock, ::urbi::UVar &uvar);
   
    virtual void start();

    template<typename T>
    void write(T value) {
        std::lock_guard<std::mutex> lck(*lock_);

        writeRecordBegin();

        value.serialize(*file_);
        *file_ << std::endl;
    }
private:
    std::string syntacticType_;
};


};
}
}


#endif	/* VectorWriter_HPP */

