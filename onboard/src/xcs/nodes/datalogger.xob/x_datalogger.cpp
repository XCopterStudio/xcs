#include "x_datalogger.hpp"

#include <sstream>

#include <boost/bind.hpp>

#include <xcs/nodes/xobject/x.h>
#include <xcs/logging.hpp>


using namespace std;
using namespace std::chrono;
using namespace ::urbi;
using namespace xcs::nodes;
using namespace xcs::nodes::datalogger;

const char* XDatalogger::REGISTER = "register";

xcs::SyntacticCategoryMap XDatalogger::syntacticCategoryMap_;

// =================================== public functions ====================

XDatalogger::XDatalogger(const std::string& name) :
  xcs::nodes::XObject(name),
  inited_(false) {
    fillTypeCategories(syntacticCategoryMap_);

    XBindFunction(XDatalogger, init);
    XBindFunction(XDatalogger, start);
    XBindFunction(XDatalogger, registerData);
    XBindFunction(XDatalogger, registerVideo);
}

XDatalogger::~XDatalogger() {
    context_.file.close();
}

void XDatalogger::start() {
    closeHeader();
    context_.enabled = true;
}

void XDatalogger::init(const std::string &file) {
    context_.file.open(file.c_str());

    if (!context_.file.is_open()) {
        send("throw \"Datalogger cannot open file: " + file + "\";");
        return;
    }
    inited_ = true;
}

void XDatalogger::registerData(const std::string &name, const std::string &semanticType, const std::string &syntacticType, ::urbi::UVar &uvar) {
    if (!inited_ || !validRegister()) {
        return;
    }

    SyntacticCategoryType syntacticCategory = syntacticCategoryMap_.at(syntacticType);
    if (syntacticCategory == CATEGORY_VIDEO) {
        send("throw \"Use registerVideo function for video data (" + name + ")\";");
        return;
    }

    registerHeader(name, semanticType, syntacticType);

    /*
     * Create writer for specied category of types.
     */
    AbstractWriter* writer = nullptr;
    switch (syntacticCategory) {
        case CATEGORY_SCALAR:
            writer = new GeneralWriter(std::string());
            dynamic_cast<GeneralWriter *>(writer)->init(name, context_, uvar);
            generalWriterList_.push_back(std::unique_ptr<GeneralWriter>(dynamic_cast<GeneralWriter *>(writer)));
            break;
        case CATEGORY_VECTOR:
            writer = new VectorWriter(std::string());
            dynamic_cast<VectorWriter *>(writer)->init(syntacticType, name, context_, uvar);
            vectorWriterList_.push_back(std::unique_ptr<VectorWriter>(dynamic_cast<VectorWriter *>(writer)));
            break;
        default:
            XCS_LOG_WARN("Category unhandled " << syntacticCategory);
            break;
    }
}

void XDatalogger::registerVideo(const std::string &videoFile, int width, int height, const std::string &name, const std::string &semanticType, const std::string &syntacticType, ::urbi::UVar &uvar) {
    if (!inited_ || !validRegister()) {
        return;
    }

    SyntacticCategoryType syntacticCategory = syntacticCategoryMap_.at(syntacticType);
    if (syntacticCategory != CATEGORY_VIDEO) {
        send("throw \"Use registerData function for non-video data (" + name + ")\";");
        return;
    }

    registerHeader(name, semanticType, syntacticType);
    VideoWriter* function = new VideoWriter(std::string());
    function->init(videoFile, width, height, name, context_, uvar);
    videoWriterList_.push_back(std::unique_ptr<VideoWriter>(function));
}

XStart(XDatalogger);