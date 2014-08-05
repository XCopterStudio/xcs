#ifndef VectorWriter_HPP
#define	VectorWriter_HPP

#include <string>
#include <urbi/uobject.hh>

#include "abstract_writer.hpp"
#include <xcs/types/type_utils.hpp>

namespace xcs {
namespace nodes {
namespace datalogger {

class VectorWriter : public AbstractWriter {
public:
    VectorWriter(const std::string &name);
    void init(const std::string &syntacticType, const std::string &dataName, LoggerContext &context, ::urbi::UVar &uvar);

    template<typename T>
    void write(T value) {
        if (!context_->enabled || !enabled()) {
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

