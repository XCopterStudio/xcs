#include "datalogger.hpp"

#include <sstream>

#include <boost/bind.hpp>

using namespace std;
using namespace ::urbi;
using namespace xcs::nodes;

const char* Datalogger::REGISTER = "register";

// =================================== public functions ====================

FileWriterFunction::FileWriterFunction(const std::string &name) :
UObject(name)
{
    UBindFunction(FileWriterFunction, init);
}

void FileWriterFunction::init(const std::string &dataName, std::ofstream* file, ::urbi::UVar &uvar){
    file_ = file;
    dataName_ = dataName;
    UNotifyChange(uvar, &FileWriterFunction::write);
}

void FileWriterFunction::write(urbi::UVar &uvar){
    cerr << dataName_ << " " << uvar.val() << endl;
    *file_ << dataName_ << " " << uvar.val() << endl;
}

Datalogger::Datalogger(const std::string& name) :
xcs::nodes::XObject(name)
{
    XBindFunction(Datalogger, init);
    XBindFunction(Datalogger, registerData);
}

Datalogger::~Datalogger(){
    file_.close();
}

void Datalogger::init(const std::string &file){
    file_.open(file.c_str());

    if (!file_.is_open()){
        cerr << "Datalogger cannot open file: " << file << endl;
        return;
    }
}

void Datalogger::registerData(const std::string &name, const std::string &semanticType, const std::string &syntacticType, ::urbi::UVar &uvar){
    if (!file_.is_open()){
        cerr << "File error" << endl;
    }
    else{
        file_ << REGISTER << " " << name << " " << semanticType << " " << syntacticType << endl;
        FileWriterFunction* function = new FileWriterFunction(std::string());
        function->init(name, &file_, uvar);
        writerList_.push_back(std::unique_ptr<FileWriterFunction>(function));
    }
}

XStart(FileWriterFunction);
XStart(Datalogger);