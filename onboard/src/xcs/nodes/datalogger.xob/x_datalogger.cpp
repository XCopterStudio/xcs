#include "x_datalogger.hpp"

#include <sstream>

#include <boost/bind.hpp>
#include <boost/filesystem/path.hpp>

#include <xcs/nodes/xobject/x.h>
#include <xcs/logging.hpp>

#include <xcs/nodes/dataplayer.xob/x_dataplayer.hpp>

using namespace std;
using namespace std::chrono;
using namespace ::urbi;
using namespace xcs::nodes;
using namespace xcs::nodes::datalogger;
using namespace boost::filesystem;

const char* XDatalogger::REGISTER = "register";

xcs::SyntacticCategoryMap XDatalogger::syntacticCategoryMap_;

// =================================== public functions ====================

XDatalogger::XDatalogger(const std::string& name) :
  xcs::nodes::XObject(name),
  inited_(false) {
    fillTypeCategories(syntacticCategoryMap_);

    XBindFunction(XDatalogger, init);
    XBindFunction(XDatalogger, registerXVar);
    XBindFunction(XDatalogger, unregisterXVar);
}

XDatalogger::~XDatalogger() {
    context_.file.flush();
    context_.file.close();
}

void XDatalogger::stateChanged(XObject::State state) {
    switch (state) {
        case XObject::STATE_STARTED:
            if (!inited_) {
                context_.file.open(filename_.c_str());

                if (!context_.file.is_open()) {
                    send("throw \"Datalogger cannot open file: " + filename_ + "\";");
                    return;
                }
                inited_ = true;
                headerClosed_ = false;
            }

            context_.enabled = true;
            closeHeader();
            break;
        case XObject::STATE_STOPPED:
            context_.file.flush();
            context_.enabled = false;
            break;
    }
}

void XDatalogger::init(const std::string &file) {
    filename_ = file;
}

void XDatalogger::registerXVar(const std::string &name, const std::string &semanticType, const std::string &syntacticType, ::urbi::UVar &uvar) {
    if (xVarToWriterMap_.count(name)) {
        if (xVarToNameMap_[uvar.get_name()] != name) {
            XCS_LOG_WARN("The XVar is already registered with different name '" << xVarToNameMap_[uvar.get_name()] << "' instead of '" << name << "'.");
        }
        xVarToWriterMap_[name]->enabled(true);
        return;
    }

    SyntacticCategoryType syntacticCategory = syntacticCategoryMap_.at(syntacticType);
    switch (syntacticCategory) {
        case CATEGORY_VIDEO:
            registerVideo(640, 360, name, semanticType, syntacticType, uvar);
            break;
        default:
            registerData(name, semanticType, syntacticType, uvar);
            break;
    }
}

void XDatalogger::unregisterXVar(const std::string& name) {
    if (!xVarToWriterMap_.count(name)) {
        XCS_LOG_ERROR("XVar '" << name << "' doesn't exist.");
        return;
    }
    xVarToWriterMap_[name]->enabled(false);
}

void XDatalogger::registerData(const std::string &name, const std::string &semanticType, const std::string &syntacticType, ::urbi::UVar &uvar) {
    if (!XDataplayer::isChannelNameValid(name)) {
        XCS_LOG_WARN("Invalid channel name '" << name << "'.");
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
            writer = new ScalarWriter(std::string());
            dynamic_cast<ScalarWriter *> (writer)->init(name, context_, uvar);
            generalWriterList_.push_back(std::unique_ptr<ScalarWriter>(dynamic_cast<ScalarWriter *> (writer)));
            break;
        case CATEGORY_VECTOR:
            writer = new VectorWriter(std::string());
            dynamic_cast<VectorWriter *> (writer)->init(syntacticType, name, context_, uvar);
            vectorWriterList_.push_back(std::unique_ptr<VectorWriter>(dynamic_cast<VectorWriter *> (writer)));
            break;
        default:
            XCS_LOG_WARN("Category unhandled " << syntacticCategory);
            break;
    }

    xVarToWriterMap_.emplace(name, writer);
    xVarToNameMap_.emplace(uvar.get_name(), name);
}

void XDatalogger::registerVideo(int width, int height, const std::string &name, const std::string &semanticType, const std::string &syntacticType, ::urbi::UVar &uvar) {
    if (!XDataplayer::isChannelNameValid(name)) {
        XCS_LOG_WARN("Invalid channel name '" << name << "'.");
        return;
    }

    SyntacticCategoryType syntacticCategory = syntacticCategoryMap_.at(syntacticType);
    if (syntacticCategory != CATEGORY_VIDEO) {
        send("throw \"Use registerData function for non-video data (" + name + ")\";");
        return;
    }

    path videoFile(filename_);
    videoFile.replace_extension(); // remove
    videoFile += "-" + name;
    videoFile += ".avi";

    registerHeader(name, semanticType, syntacticType);
    VideoWriter* writer = new VideoWriter(std::string());
    writer->init(videoFile.string(), width, height, name, context_, uvar);
    videoWriterList_.push_back(std::unique_ptr<VideoWriter>(writer));

    xVarToWriterMap_.emplace(name, writer);
    xVarToNameMap_.emplace(uvar.get_name(), name);
}

XStart(XDatalogger);