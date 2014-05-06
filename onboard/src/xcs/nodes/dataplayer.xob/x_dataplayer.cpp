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
        stringstream sline(line);
        string check, name, semanticType, synType;
        sline >> check >> name >> semanticType >> synType;

        if (check != "register") {
            continue;
        }

        SimpleXVar &xvar = dataReceiver_.registerOutput(name, XType(synType, semanticType, XType::DATAFLOWTYPE_XVAR));
        XBindVarRename(xvar, name);

        channelTypes_[name] = synType;

        // create reader
        // channelName -> reader

        cerr << "Got: " << name << endl;

    }

    XCS_LOG_INFO("Loaded header...");
}

void XDataplayer::loop() {
    // load initial record

    while (1) {
        this_thread::sleep_for(chrono::milliseconds(100));
        if (!isPlaying_) {
            continue;
        }

        //string line;
        //getline(file_, line);
        //stringstream sline(line);
        string timestamp, name, rest;

        file_ >> timestamp >> name;
        if (file_.eof()) {
            break;
        }

        string synType;
        //getline(sline, rest);
        try {
            synType = channelTypes_.at(name);
        } catch (out_of_range &e) {
            // TODO only read the line and continue on the next one, with warning and not breaking
            XCS_LOG_ERROR("Unknown channel " << name << ".");
            break;
        }
        auto categoryType = syntacticCategoryMap_.at(synType);
        FrameInfo frameInfo;

        switch (categoryType) {
            case CATEGORY_SCALAR:
#define FILTER(Type) if(synType == #Type) {Type tmp; file_ >> tmp; dataReceiver_.notify(name, tmp); }
                LIBPORT_LIST_APPLY(FILTER, XCS_SCALAR_TYPES);
#undef FILTER
                break;

            case CATEGORY_VECTOR:
#define FILTER(Type) if (synType == #Type) { dataReceiver_.notify(name, Type::deserialize(file_)); }
                LIBPORT_LIST_APPLY(FILTER, XCS_VECTOR_TYPES);
#undef FILTER
                break;

            case CATEGORY_VIDEO:

#define FILTER(Type) if (synType == #Type) {\
        frameInfo = Type::deserialize(file_);\
        Type frame = getFrame<Type>(frameInfo);\
        dataReceiver_.notify(name, frame);\
        }
                LIBPORT_LIST_APPLY(FILTER, XCS_VIDEO_TYPES);
#undef FILTER
                break;
            default:
                XCS_LOG_WARN("Player: unhadled type category " << categoryType << ".");
                break;
        }

        //cerr << "Got " << timestamp << ":" << name << ", rest:" << rest << endl;

        // parse record

        // notify record (if video, take it from video buffer)

        // read next record        

        // sleep until next record (consider already passed time)
    }
    XCS_LOG_INFO("Log play finished.");
}

void XDataplayer::onCommand(const std::string &command) {
    if (command == CMD_PLAY) {
        isPlaying_ = true;
    } else if (command == CMD_PAUSE) {
        isPlaying_ = false;
    }
}

XStart(XDataplayer);
