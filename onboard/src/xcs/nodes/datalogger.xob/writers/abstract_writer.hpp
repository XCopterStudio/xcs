#ifndef ABSTRACTWRITER_HPP
#define	ABSTRACTWRITER_HPP

#include <string>
#include <atomic>
#include <urbi/uobject.hh>
#include <clocale>
#include <xcs/logging.hpp>

#include "../logger_context.hpp"


namespace xcs {
namespace nodes {
namespace datalogger {

class AbstractWriter : public ::urbi::UObject {
private:
    std::atomic<bool> enabled_;
protected:
    Clock highResolutionClock_;
    LoggerContext* context_;
    std::string dataName_;

    void basicInit(const std::string &dataName, LoggerContext &context, ::urbi::UVar &uvar);
    void writeRecordBegin();
public:

    AbstractWriter(const std::string &name) : urbi::UObject(name),
        enabled_(true) {
        //! decimal dot will be US .
        if (std::setlocale(LC_NUMERIC, "C") == NULL){
            XCS_LOG_WARN("Writer cannot load POSIX locale.");
        }
    }

    virtual ~AbstractWriter() {
    }
    
    inline void enabled(bool value) {
        enabled_ = value;
    }
    
    inline bool enabled() const {
        return enabled_;
    }
};

};
}
}


#endif	/* ABSTRACTWRITER_HPP */

