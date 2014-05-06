#include "x_dataplayer.hpp"

#include <xcs/logging.hpp>
#include <xcs/nodes/xobject/x.h>
#include <xcs/nodes/xobject/x_type.hpp>

#include <sstream>
#include <thread>

using namespace std;
using namespace xcs;
using namespace xcs::nodes;


const std::string XDataplayer::CMD_PLAY = "Play";
const std::string XDataplayer::CMD_PAUSE = "Pause";

xcs::SyntacticCategoryMap XDataplayer::syntacticCategoryMap_;

XDataplayer::XDataplayer(const std::string& name) :
  xcs::nodes::XObject(name),
  command("COMMAND"),
  seek("INTEGER"),
  isPlaying_(false) {
    fillTypeCategories(syntacticCategoryMap_);

    XBindFunction(XDataplayer, init);
    XBindVarF(command, &XDataplayer::onCommand);
}

XDataplayer::~XDataplayer() {
    file_.close();
}

void XDataplayer::init(const std::string &file) {
    file_.open(file.c_str());

    if (!file_.is_open()) {
        send("throw \"Dataplayer cannot open file: " + file + "\";");
        return;
    }

    loadHeader();

    dataLoopThread_ = move(thread(&XDataplayer::loop, this));
}

void XDataplayer::loadHeader() {
    while (true) {
        string line;
        getline(file_, line);
        if (line.empty()) {
            break;
        }
        processHeaderLine(line);
    }

    XCS_LOG_INFO("Loaded header of log.");
}

void XDataplayer::processHeaderLine(const std::string &line) {
    stringstream sline(line);
    string check, name, semanticType, synType;
    sline >> check >> name >> semanticType >> synType;

    if (check != "register") {
        XCS_LOG_WARN("Unrecognized header '" << check << "'.");
        return;
    }

    SimpleXVar &xvar = dataReceiver_.registerOutput(name, XType(synType, semanticType, XType::DATAFLOWTYPE_XVAR));
    XBindVarRename(xvar, name);

    channelTypes_[name] = synType;

}

void XDataplayer::loop() {
    string timestamp, name;
    Timestamp ts(0), prevTs(0);

    while (1) {
        if (!isPlaying_) {
            continue;
        }
        // load record header
        file_ >> timestamp >> name;
        if (file_.eof()) {
            break;
        }

        // wait for it
        ts = stol(timestamp);
        this_thread::sleep_for(chrono::milliseconds(ts - prevTs)); //TODO (consider already passed time)

        // notify it
        processLogLine(name, ts);
        prevTs = ts;
    }
    XCS_LOG_INFO("Log play finished.");
}

void XDataplayer::processLogLine(const std::string &channel, const Timestamp timestamp) {
    string synType;
    try {
        synType = channelTypes_.at(channel);
    } catch (out_of_range &e) {
        // TODO only read the line and continue on the next one, with warning and not breaking
        XCS_LOG_ERROR("Unknown channel " << channel << ".");
    }
    auto categoryType = syntacticCategoryMap_.at(synType);
    FrameInfo frameInfo;

    switch (categoryType) {
        case CATEGORY_SCALAR:
#define FILTER(Type) if(synType == #Type) {Type tmp; file_ >> tmp; dataReceiver_.notify(channel, tmp); }
            LIBPORT_LIST_APPLY(FILTER, XCS_SCALAR_TYPES);
#undef FILTER
            break;

        case CATEGORY_VECTOR:
#define FILTER(Type) if (synType == #Type) { dataReceiver_.notify(channel, Type::deserialize(file_)); }
            LIBPORT_LIST_APPLY(FILTER, XCS_VECTOR_TYPES);
#undef FILTER
            break;

        case CATEGORY_VIDEO:

#define FILTER(Type) if (synType == #Type) {\
        frameInfo = Type::deserialize(file_);\
        Type frame = getFrame<Type>(frameInfo);\
        dataReceiver_.notify(channel, frame);\
        }
            LIBPORT_LIST_APPLY(FILTER, XCS_VIDEO_TYPES);
#undef FILTER
            break;
        default:
            XCS_LOG_WARN("Player: unhandled type category " << categoryType << ".");
            break;
    }
}

void XDataplayer::onCommand(const std::string &command) {
    if (command == CMD_PLAY) {
        isPlaying_ = true;
    } else if (command == CMD_PAUSE) {
        isPlaying_ = false;
    }
}

XStart(XDataplayer);
