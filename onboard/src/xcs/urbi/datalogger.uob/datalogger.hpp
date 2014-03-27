#ifndef DATALOGGER_H
#define DATALOGGER_H

#include <string>
#include <memory>
#include <list>
#include <chrono>
#include <fstream>
#include <mutex>

#include <xcs/nodes/xobject/x_object.hpp>
#include <xcs/nodes/xobject/x_var.hpp>
#include <xcs/nodes/xobject/x.hpp>
#include <xcs/nodes/xobject/x_input_port.hpp>
#include "video_file_writer.hpp"

namespace xcs{
namespace nodes{
    typedef std::chrono::high_resolution_clock Clock;
    typedef std::chrono::time_point<std::chrono::high_resolution_clock> TimePoint;

    class DataWriter : public xcs::nodes::XObject{
        Clock highResolutionClock_;
        TimePoint startTime_;
        std::string dataName_;
        std::ofstream *file_;

        std::mutex *lock_;
    public:
        DataWriter(const std::string &name);
        void init(const std::string &dataName, const TimePoint startTime, std::ofstream* file, std::mutex *lock, ::urbi::UVar &uvar);
        void write(::urbi::UVar &uvar);
    };

    class VideoWriter : public xcs::nodes::XObject{
        Clock highResolutionClock_;
        TimePoint startTime_;
        std::string dataName_;
        std::ofstream *file_;

        unsigned int frameNumber_;
        AVFrame* avframe_;
        std::unique_ptr<VideoFileWriter> videoFileWriter;

        std::mutex *lock_;
    public:
        VideoWriter(const std::string &name);
        ~VideoWriter();
        void init(const std::string &videoFile, const unsigned int &width, const unsigned int &height, const std::string &dataName, const TimePoint startTime, std::ofstream* file, std::mutex *lock, ::urbi::UVar &uvar);
        void write(::urbi::UImage image);
    };

    typedef std::list<std::unique_ptr<DataWriter> > DataWriterList;
    typedef std::list<std::unique_ptr<VideoWriter> > VideoWriterList;

    class Datalogger : public xcs::nodes::XObject {
        static const char* REGISTER;

        DataWriterList dataWriterList_;
        VideoWriterList videoWriterList_;
        TimePoint startTime_;

        std::mutex lock_;

        std::ofstream file_;
    public:
        Datalogger(const std::string& name);
        ~Datalogger();
        
        void init(const std::string &file);
        void registerData(const std::string &name, const std::string &semanticType, const std::string &syntacticType, ::urbi::UVar &uvar);
        void registerVideo(const std::string &videoFile, int width, int height, const std::string &name, const std::string &semanticType, const std::string &syntacticType, ::urbi::UVar &uvar);
    };

}}

#endif