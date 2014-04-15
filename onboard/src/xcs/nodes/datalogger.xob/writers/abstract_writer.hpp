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
    TimePoint startTime_;
    std::string dataName_;
    std::ofstream *file_;

    std::mutex *lock_;
    
    ::urbi::UVar *uvar_;

    void writeRecordBegin();
public:
    virtual void init(const std::string &dataName, const TimePoint startTime, std::ofstream* file, std::mutex *lock, ::urbi::UVar &uvar);
    
    virtual void start() = 0;

    AbstractWriter(const std::string &name) : urbi::UObject(name) {
    }

    virtual ~AbstractWriter() {
    }
};

};
}
}


#endif	/* ABSTRACTWRITER_HPP */

