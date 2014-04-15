#ifndef DATALOGGER_H
#define DATALOGGER_H

#include <string>
#include <fstream>
#include <mutex>

#include <xcs/nodes/xobject/x_object.hpp>

#include "writer_common.hpp"
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

    GeneralWriterList generalWriterList_;
    VectorWriterList vectorWriterList_;
    VideoWriterList videoWriterList_;
    datalogger::TimePoint startTime_;

    std::mutex lock_;

    std::ofstream file_;

    bool inited_;

    inline void registerHeader(const std::string &name, const std::string &semanticType, const std::string &syntacticType) {
        file_ << REGISTER << " " << name << " " << semanticType << " " << syntacticType << std::endl;
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