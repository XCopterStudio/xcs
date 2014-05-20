#ifndef DATALOGGER_H
#define DATALOGGER_H

#include <string>
#include <fstream>
#include <mutex>
#include <atomic>

#include <xcs/nodes/xobject/x_object.hpp>
#include <xcs/types/type_utils.hpp>

#include "logger_context.hpp"
#include "writers/general_writer.hpp"
#include "writers/video_writer.hpp"
#include "writers/vector_writer.hpp"

namespace xcs {
namespace nodes {

class XDatalogger : public xcs::nodes::XObject {
    typedef std::list<std::unique_ptr<datalogger::GeneralWriter> > GeneralWriterList;
    typedef std::list<std::unique_ptr<datalogger::VectorWriter> > VectorWriterList;
    typedef std::list<std::unique_ptr<datalogger::VideoWriter> > VideoWriterList;

    static const char* REGISTER;

    static xcs::SyntacticCategoryMap syntacticCategoryMap_;

    GeneralWriterList generalWriterList_;
    VectorWriterList vectorWriterList_;
    VideoWriterList videoWriterList_;

    bool inited_;

    datalogger::LoggerContext context_;
    std::string filename_;

    inline void registerHeader(const std::string &name, const std::string &semanticType, const std::string &syntacticType) {
        context_.file << REGISTER << " " << name << " " << semanticType << " " << syntacticType << std::endl;
    }

    inline void closeHeader() {
        context_.file << std::endl;
    }

    inline bool validRegister() {
        if (context_.enabled) {
            send("throw \"Cannot register input in enabled logger.\";");
        }
        return !context_.enabled;
    }

public:
    XDatalogger(const std::string& name);
    virtual ~XDatalogger();

    void init(const std::string &file);
    void start();
    void registerData(const std::string &name, const std::string &semanticType, const std::string &syntacticType, ::urbi::UVar &uvar);
    void registerVideo(int width, int height, const std::string &name, const std::string &semanticType, const std::string &syntacticType, ::urbi::UVar &uvar);
};

}
}

#endif