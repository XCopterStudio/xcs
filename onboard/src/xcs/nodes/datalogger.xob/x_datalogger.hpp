#ifndef DATALOGGER_H
#define DATALOGGER_H

#include <string>
#include <fstream>
#include <sstream>
#include <mutex>
#include <atomic>
#include <map>

#include <xcs/nodes/xobject/x_object.hpp>
#include <xcs/types/type_utils.hpp>

#include "logger_context.hpp"
#include "writers/scalar_writer.hpp"
#include "writers/video_writer.hpp"
#include "writers/vector_writer.hpp"

namespace xcs {
namespace nodes {

class XDatalogger : public xcs::nodes::XObject {
    typedef std::map<std::string, datalogger::AbstractWriter*> WriterMap;
    typedef std::map<std::string, std::string> XVarNameMap;
    typedef std::list<std::unique_ptr<datalogger::ScalarWriter> > GeneralWriterList;
    typedef std::list<std::unique_ptr<datalogger::VectorWriter> > VectorWriterList;
    typedef std::list<std::unique_ptr<datalogger::VideoWriter> > VideoWriterList;

    static const char* REGISTER;

    static xcs::SyntacticCategoryMap syntacticCategoryMap_;

    WriterMap xVarToWriterMap_; //! port name => writer
    XVarNameMap xVarToNameMap_; //! UVar name => port name
    GeneralWriterList generalWriterList_;
    VectorWriterList vectorWriterList_;
    VideoWriterList videoWriterList_;

    bool inited_;
    bool headerClosed_;

    datalogger::LoggerContext context_;
    std::string filename_;
    std::stringstream buffer_;

    inline void registerHeader(const std::string &name, const std::string &semanticType, const std::string &syntacticType) {
        if (inited_) {
            context_.file << REGISTER << " " << name << " " << semanticType << " " << syntacticType << std::endl;
        } else {
            buffer_ << REGISTER << " " << name << " " << semanticType << " " << syntacticType << std::endl;
        }
    }

    inline void closeHeader() {
        if(inited_ && !headerClosed_) {
            context_.file << buffer_.str() << std::endl;
            headerClosed_ = true;
        }        
    }

    inline bool validRegister() {
        if (context_.enabled) {
            XCS_LOG_ERROR("Cannot register input in enabled logger.");
        }
        return !context_.enabled;
    }
    void registerData(const std::string &name, const std::string &semanticType, const std::string &syntacticType, ::urbi::UVar &uvar);
    void registerVideo(int width, int height, const std::string &name, const std::string &semanticType, const std::string &syntacticType, ::urbi::UVar &uvar);
protected:
    virtual void stateChanged(XObject::State state);
public:
    XDatalogger(const std::string& name);
    virtual ~XDatalogger();

    void init(const std::string &file);
    void registerXVar(const std::string &name, const std::string &semanticType, const std::string &syntacticType, ::urbi::UVar &uvar);
    void unregisterXVar(const std::string &name);
};

}
}

#endif