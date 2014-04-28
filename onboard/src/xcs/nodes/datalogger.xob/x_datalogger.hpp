#ifndef DATALOGGER_H
#define DATALOGGER_H

#include <string>
#include <fstream>
#include <mutex>
#include <atomic>

#include <xcs/nodes/xobject/x_object.hpp>

#include "writer_common.hpp"
#include "writers/general_writer.hpp"
#include "writers/video_writer.hpp"
#include "writers/vector_writer.hpp"

namespace xcs {
namespace nodes {

class XDatalogger : public xcs::nodes::XObject {
    typedef std::list<std::unique_ptr<datalogger::GeneralWriter, std::function<void (datalogger::GeneralWriter *)> > > GeneralWriterList;
    typedef std::list<std::unique_ptr<datalogger::VectorWriter> > VectorWriterList;
    typedef std::list<std::unique_ptr<datalogger::VideoWriter> > VideoWriterList;

    static const char* REGISTER;

    GeneralWriterList generalWriterList_;
    VectorWriterList vectorWriterList_;
    VideoWriterList videoWriterList_;

    bool inited_;
    
    datalogger::LoggerContext context_;

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

    inline bool isVideoType(const std::string &syntacticType) const {
        return syntacticType == "xcs::BitmapType" || syntacticType == "xcs::BitmapTypeChronologic";
    }

    inline bool isVectorType(const std::string &syntacticType) const {
#define DECLARE(Type) || (syntacticType == #Type)
        return false LIBPORT_LIST_APPLY(DECLARE, VECTOR_TYPES);
    }
public:
    XDatalogger(const std::string& name);
    ~XDatalogger();

    void init(const std::string &file);
    void start();
    void registerData(const std::string &name, const std::string &semanticType, const std::string &syntacticType, ::urbi::UVar &uvar);
    void registerVideo(const std::string &videoFile, int width, int height, const std::string &name, const std::string &semanticType, const std::string &syntacticType, ::urbi::UVar &uvar);
};

}
}

#endif