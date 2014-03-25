#ifndef DATALOGGER_H
#define DATALOGGER_H

#include <string>
#include <memory>
#include <list>
#include <chrono>
#include <fstream>

#include <xcs/nodes/xobject/x_object.hpp>
#include <xcs/nodes/xobject/x_var.hpp>
#include <xcs/nodes/xobject/x.hpp>
#include <xcs/nodes/xobject/x_input_port.hpp>

namespace xcs{
namespace nodes{
    typedef std::chrono::high_resolution_clock Clock;
    typedef std::chrono::time_point<std::chrono::high_resolution_clock> TimePoint;

    class DataWriter : public xcs::nodes::XObject{
    protected:
        Clock highResolutionClock_;
        TimePoint startTime_;
        std::string dataName_;
        std::ofstream *file_;
    public:
        DataWriter(const std::string &name);
        void init(const std::string &dataName, const TimePoint startTime, std::ofstream* file, ::urbi::UVar &uvar);
        virtual void write(::urbi::UVar &uvar);
    };

    class VideoWriter : public DataWriter{
    public:
        VideoWriter(const std::string &name);
        void init(const std::string &videoFile, const std::string &dataName, const TimePoint startTime, std::ofstream* file, ::urbi::UVar &uvar);
        virtual void write(::urbi::UVar &uvar);
    };

    typedef std::list<std::unique_ptr<DataWriter> > WriterList;

    class Datalogger : public xcs::nodes::XObject {
        static const char* REGISTER;

        WriterList writerList_;
        TimePoint startTime_;

        std::ofstream file_;
    public:
        Datalogger(const std::string& name);
        ~Datalogger();
        
        void init(const std::string &file);
        void registerData(const std::string &name, const std::string &semanticType, const std::string &syntacticType, ::urbi::UVar &uvar);
        void registerVideo(const std::string &videoFile, const std::string &name, const std::string &semanticType, const std::string &syntacticType, ::urbi::UVar &uvar);
    };

}}

#endif