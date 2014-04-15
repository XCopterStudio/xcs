#include "x_datalogger.hpp"

#include <sstream>

#include <boost/bind.hpp>

#include <xcs/nodes/xobject/x.hpp>


using namespace std;
using namespace std::chrono;
using namespace ::urbi;
using namespace xcs::nodes;
using namespace xcs::nodes::datalogger;

const char* XDatalogger::REGISTER = "register";

// =================================== public functions ====================

XDatalogger::XDatalogger(const std::string& name) :
  xcs::nodes::XObject(name),
  startTime_(Clock().now()),
  inited_(false) {
    XBindFunction(XDatalogger, init);
    XBindFunction(XDatalogger, start);
    XBindFunction(XDatalogger, registerData);
    XBindFunction(XDatalogger, registerVideo);
}

XDatalogger::~XDatalogger() {
    file_.close();
}

void XDatalogger::start() {
    cerr << "Beg" << endl;
    for (auto it = generalWriterList_.begin(); it != generalWriterList_.end(); ++it) {
        (*it).get()->start();
    }
    cerr << "Beg1" << endl;
    for (auto it = vectorWriterList_.begin(); it != vectorWriterList_.end(); ++it) {
        (*it).get()->start();
    }
    cerr << "Beg2" << endl;
    for (auto it = videoWriterList_.begin(); it != videoWriterList_.end(); ++it) {
        (*it).get()->start();
    }
    cerr << "Beg3" << endl;
}

void XDatalogger::init(const std::string &file) {
    file_.open(file.c_str());

    if (!file_.is_open()) {
        send("throw \"Datalogger cannot open file: " + file + "\";");
        return;
    }
    inited_ = true;
}

void XDatalogger::registerData(const std::string &name, const std::string &semanticType, const std::string &syntacticType, ::urbi::UVar &uvar) {
    if (!inited_) {
        return;
    }

    if (isVideoType(syntacticType)) {
        send("throw \"Use registerVideo function for video data (" + name + ")\";");
        return;
    }

    registerHeader(name, semanticType, syntacticType);

    if (isVectorType(syntacticType)) {
        cerr << "Registered vector writer " << endl;
        VectorWriter* function = new VectorWriter(std::string());
        function->init(syntacticType, name, startTime_, &file_, &lock_, uvar);
        vectorWriterList_.push_back(std::unique_ptr<VectorWriter>(function));
    } else {
        cerr << "Registered general writer " << endl;
        GeneralWriter* function = new GeneralWriter(std::string());
        function->init(name, startTime_, &file_, &lock_, uvar);
        generalWriterList_.push_back(std::unique_ptr<GeneralWriter>(function));
    }



}

void XDatalogger::registerVideo(const std::string &videoFile, int width, int height, const std::string &name, const std::string &semanticType, const std::string &syntacticType, ::urbi::UVar &uvar) {
    if (!inited_) {
        return;
    }

    if (!isVideoType(syntacticType)) {
        send("throw \"Use registerData function for non-video data (" + name + ")\";");
        return;
    }

    registerHeader(name, semanticType, syntacticType);
    VideoWriter* function = new VideoWriter(std::string());
    function->init(videoFile, width, height, name, startTime_, &file_, &lock_, uvar);
    videoWriterList_.push_back(std::unique_ptr<VideoWriter>(function));
}

XStart(XDatalogger);