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

const size_t XDataplayer::PRELOAD_OFFSET;
const size_t XDataplayer::IDLE_SLEEP = 100;

const std::string XDataplayer::DUMMY_VIDEO_CHANNEL = ".dummy";

xcs::SyntacticCategoryMap XDataplayer::syntacticCategoryMap_;

XDataplayer::XDataplayer(const std::string& name) :
  xcs::nodes::XObject(name),
  /*command("COMMAND"),*/
  finished("EVENT"),
  isPlaying_(false),
  endAll_(false) {
    fillTypeCategories(syntacticCategoryMap_);

    XBindFunction(XDataplayer, init);
    
    //TODO: there should be usefull to add command REPLAY...
    //XBindVarF(command, &XDataplayer::onCommand);

    XBindVar(finished);

    finished = false;
    dataReceiver_.enabled(true);
}

XDataplayer::~XDataplayer() {
    endAll_ = true;
    videoJobs_.push(VideoJob(DUMMY_VIDEO_CHANNEL, 0)); // this will unblock the decoder thread if its queue is empty
    file_.close();

    if (dataLoopThread_.joinable()) {
        dataLoopThread_.join();
    }
    if (videoDecodeThread_.joinable()) {
        videoDecodeThread_.join();
    }
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

void XDataplayer::stateChanged(XObject::State state) {
    switch (state) {
        case XObject::STATE_STARTED:
            play();
            break;
        case XObject::STATE_STOPPED:
            pause();
            break;
    }
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

    if (!isChannelNameValid(channelName)) {
        XCS_LOG_WARN("Invalid channel name '" << channelName << "'.");
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
    Timestamp ts(0);

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

        auto currentTime = Clock::now();
        int sleepTime = ts * 1000000 - std::chrono::duration_cast<std::chrono::microseconds>(currentTime - startTime_).count();
        // wait for it
        if (sleepTime > 0) {
            this_thread::sleep_for(chrono::microseconds(sleepTime));
        }

        // notify it
        processLogLine(name, ts);
    }
    if (!endAll_) {
        finished = true;
        XCS_LOG_INFO("Log play finished.");
    }
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
        if (channel == DUMMY_VIDEO_CHANNEL) {
            break;
        }
        // Frame number is provisionally ignored, until seeking is needed. 
        //auto frameNumber = job.second; // frame
        videoResults_.at(channel)->push(videoPlayers_.at(channel)->getFrame());
    }
}

//void XDataplayer::onCommand(const std::string &command) {
//    XCS_LOG_WARN("No dataplayer commands implemented."); // NOTE: possible reset of the player
//}

void XDataplayer::play() {
    if (!paused) {
        startTime_ = Clock::now();
    } else {
        startTime_ += (Clock::now() - pausedTime_);
        paused = false;
    }
    isPlaying_ = true;
}

void XDataplayer::pause() {
    isPlaying_ = false;
    pausedTime_ = Clock::now();
    paused = true;
}

XStart(XDataplayer);
