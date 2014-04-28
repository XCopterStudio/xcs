#ifndef ABSTRACTWRITER_HPP
#define	ABSTRACTWRITER_HPP

#include <string>
#include <urbi/uobject.hh>

#include "../writer_common.hpp"


namespace xcs {
namespace nodes {
namespace datalogger {

class AbstractWriter : public ::urbi::UObject {
protected:
    Clock highResolutionClock_;
    LoggerContext* context_;
    std::string dataName_;

    void basicInit(const std::string &dataName, LoggerContext &context, ::urbi::UVar &uvar);
    void writeRecordBegin();
public:

    AbstractWriter(const std::string &name) : urbi::UObject(name) {
    }

    virtual ~AbstractWriter() {
    }
};

};
}
}


#endif	/* ABSTRACTWRITER_HPP */

