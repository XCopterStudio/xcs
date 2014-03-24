#ifndef DATALOGGER_H
#define DATALOGGER_H

#include <string>
#include <memory>
#include <list>
#include <fstream>

#include <xcs/nodes/xobject/x_object.hpp>
#include <xcs/nodes/xobject/x_var.hpp>
#include <xcs/nodes/xobject/x_input_port.hpp>

namespace xcs{
namespace nodes{
    class FileWriterFunction : public ::urbi::UObject{
        std::string dataName_;
        std::ofstream *file_;
    public:
        FileWriterFunction(const std::string &name);
        void init(const std::string &dataName, std::ofstream* file, ::urbi::UVar &uvar);
        void write(::urbi::UVar &uvar);
    };

    typedef std::list<std::unique_ptr<FileWriterFunction> > WriterList;

    class Datalogger : public xcs::nodes::XObject {
        static const char* REGISTER;

        WriterList writerList_;

        std::ofstream file_;
    public:
        Datalogger(const std::string& name);
        ~Datalogger();
        
        void init(const std::string &file);
        void registerData(const std::string &name, const std::string &semanticType, const std::string &syntacticType, ::urbi::UVar &uvar);
    };

}}

#endif