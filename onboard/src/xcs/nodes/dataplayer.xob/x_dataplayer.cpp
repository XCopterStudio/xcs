#include "x_dataplayer.hpp"

#include <xcs/logging.hpp>
#include <xcs/nodes/xobject/x.h>
#include <xcs/nodes/xobject/x_type.hpp>

#include <boost/filesystem/path.hpp>

#include <sstream>
#include <thread>

using namespace std;
using namespace boost::filesystem;
using namespace xcs;
using namespace xcs::nodes;
using namespace xcs::nodes::dataplayer;


const std::string XDataplayer::CMD_PLAY = "Play";
const std::string XDataplayer::CMD_PAUSE = "Pause";

const size_t XDataplayer::PRELOAD_OFFSET;
const size_t XDataplayer::IDLE_SLEEP;

xcs::SyntacticCategoryMap XDataplayer::syntacticCategoryMap_;

XDataplayer::XDataplayer(const std::string& name) :
  xcs::nodes::XObject(name),
  command("COMMAND"),
  seek("INTEGER"),
  isPlaying_(false),
  endAll_(false) {
    fillTypeCategories(syntacticCategoryMap_);

    XBindFunction(XDataplayer, init);
    XBindVarF(command, &XDataplayer::onCommand);
}

XDataplayer::~XDataplayer() {
    endAll_ = true;
    file_.close();
}

void XDataplayer::init(const std::string &filename) {
    file_.open(filename.c_str());

    if (!file_.is_open()) {
        send("throw \"Dataplayer cannot open file: " + filename + "\";");
        return;
    }
    filename_ = filename;

    loadHeader();

    dataLoopThread_ = move(thread(&XDataplayer::loop, this));
    videoDecodeThread_ = move(thread(&XDataplayer::videoDecoder, this));

}

void XDataplayer::loadHeader() {
    while (true) {
        string line;
        getline(file_, line);
        if (line.empty() || line == "\r") {
            break;
        }
        processHeaderLine(line);
    }

    XCS_LOG_INFO("Loaded header of log.");
}

void XDataplayer::processHeaderLine(const std::string &line) {
    stringstream sline(line);
    string check, channelName, semanticType, synType;
    sline >> check >> channelName >> semanticType >> synType;

    if (check != "register") {
        XCS_LOG_WARN("Unrecognized header '" << check << "'.");
        return;
    }

    SimpleXVar &xvar = dataReceiver_.registerOutput(channelName, XType(synType, semanticType, XType::DATAFLOWTYPE_XVAR));
    XBindVarRename(xvar, channelName);

    channelTypes_[channelName] = synType;

    auto categoryType = syntacticCategoryMap_.at(synType);
    if (categoryType == CATEGORY_VIDEO) {
        createVideoPlayer(channelName);
    }
}

void XDataplayer::loop() {
    string name;
    Timestamp ts(0), prevTs(0);

    while (!endAll_) {
        if (!isPlaying_) {
            this_thread::sleep_for(chrono::milliseconds(IDLE_SLEEP));
            continue;
        }
        // load record header
        file_ >> ts >> name;
        if (file_.eof()) {
            break;
        }

        // wait for it
        this_thread::sleep_for(chrono::milliseconds(static_cast<int> ((ts - prevTs) * 1000))); //TODO (consider already passed time)

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
        notifyFrame<Type>(channel, frameInfo);\
        }
            LIBPORT_LIST_APPLY(FILTER, XCS_VIDEO_TYPES);
#undef FILTER
            break;
        default:
            XCS_LOG_WARN("Player: unhandled type category " << categoryType << ".");
            break;
    }
}

void XDataplayer::createVideoPlayer(const std::string& channel) {
    path videoFile(filename_);
    videoFile.replace_extension();
    videoFile += "-" + channel + ".avi";

    videoPlayers_.emplace(channel, unique_ptr<VideoPlayer>(new VideoPlayer(videoFile.string())));
    videoResults_.emplace(channel, unique_ptr<VideoResultQueue>(new VideoResultQueue()));

    for (auto i = 0; i < PRELOAD_OFFSET; ++i) {
        videoJobs_.push(VideoJob(channel, i));
    }
}

void XDataplayer::videoDecoder() {
    while (!endAll_) {
        auto job = videoJobs_.pop();
        auto channel = job.first;
        // Frame number is provisionally ignored, until seeking is needed. 
        //auto frameNumber = job.second; // frame
        videoResults_.at(channel)->push(videoPlayers_.at(channel)->getFrame());
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
