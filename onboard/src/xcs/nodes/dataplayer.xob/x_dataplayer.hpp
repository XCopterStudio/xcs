#ifndef DATALOGGER_H
#define DATALOGGER_H

#include <string>
#include <fstream>
#include <mutex>
#include <thread>
#include <cstdint>
#include <atomic>
#include <memory>
#include <chrono>

#include <xcs/nodes/xobject/x_object.hpp>
#include <xcs/nodes/xobject/x_input_port.hpp>
#include <xcs/nodes/xobject/x_var.hpp>
#include <xcs/types/timestamp.hpp>
#include <xcs/types/type_utils.hpp>
#include <xcs/xci/data_receiver.hpp>
#include <xcs/tsqueue.hpp>

#include "video_player.hpp"


namespace xcs {
namespace nodes {

namespace dataplayer {

// TODO remove?

enum PlaybackMode {
    PLAYBACK_SMOOTH, //! speed scaling is done smoothly,
    PLAYBACK_SKIPPED // speed scaling is done by skipping samples
};
};

class XDataplayer : public xcs::nodes::XObject {
public:

    XDataplayer(const std::string& name);
    virtual ~XDataplayer();

    xcs::nodes::XInputPort<std::string> command;
    xcs::nodes::XInputPort<xcs::Timestamp> seek;

    xcs::nodes::XVar<bool> finished;

    void init(const std::string &file);
private:
    typedef std::map<std::string, std::string> SyntacticMap;
    typedef std::map<std::string, std::unique_ptr<dataplayer::VideoPlayer >> VideoPlayerMap;
    typedef std::pair<std::string, size_t> VideoJob;
    typedef xcs::Tsqueue<VideoJob> VideoJobsQueue;
    typedef xcs::Tsqueue<xcs::BitmapType> VideoResultQueue;
    typedef std::map<std::string, std::unique_ptr<VideoResultQueue >> VideoResultQueueMap;
    typedef std::chrono::high_resolution_clock Clock;
    typedef std::chrono::time_point<Clock> TimePoint;

    const static std::string CMD_PLAY;
    const static std::string CMD_PAUSE;

    /*!
     * Number of video frames that are preloaded in advance (per channel).
     * Currently only 1 is supported because there are no buffers and only 
     * internal buffer of video player is exploited (it's being overwritten
     * with each new frame).for 
     */
    const static size_t PRELOAD_OFFSET = 1;

    /*!
     * Period (ms) of iteration of the main loop when player is paused.
     */
    const static size_t IDLE_SLEEP = 100;

    static xcs::SyntacticCategoryMap syntacticCategoryMap_;

    /*! Syntactic types of channels. */
    SyntacticMap channelTypes_;

    /*! Each video channel has video player.*/
    VideoPlayerMap videoPlayers_;
    VideoJobsQueue videoJobs_;
    VideoResultQueueMap videoResults_;

    dataplayer::PlaybackMode playbackMode_; //TODO remove?
    double playbackSpeed_; //TODO implement?

    std::string filename_;
    /*! Input file stream. */
    std::ifstream file_;

    /*! Do we produce data? */
    std::atomic<bool> isPlaying_;

    /*! Thread control (termination). */
    std::atomic<bool> endAll_;

    TimePoint startTime_;
    TimePoint pausedTime_;
    bool paused;


    std::thread dataLoopThread_;
    std::thread videoDecodeThread_;

    xci::DataReceiver dataReceiver_;


    //TODO?    void playbackSpeed(double value);
    //TODO?    void playbackMode(dataplayer::PlaybackMode value);
    //TODO?    void playbackSeek(xcs::TimestampType timestamp);

    void loadHeader();

    void processHeaderLine(const std::string &line);

    /*!
     * "Infinite" loops that process log (until end) and notifies data receiver.
     */
    void loop();

    void processLogLine(const std::string &channel, const Timestamp timestamp);

    void createVideoPlayer(const std::string &channel);

    /*!
     * Video decoding runs in separate thread.
     * There's one common input queue of decoding jobs (depends on log processing)
     * and multiple output queues one for each video channel.
     * Goal is to have video frame ready when log stram requires it.
     */
    void videoDecoder();

    void onCommand(const std::string &command);

    /*!
     * Obtain ready frame from correct queue, notify it and prepare another job.
     * Templating is legacy from BitmapTypeChronologic.
     */
    template<typename T>
    void notifyFrame(const std::string &channel, FrameInfo frameInfo);

};

template<>
void XDataplayer::notifyFrame<BitmapType>(const std::string &channel, FrameInfo frameInfo) {
    auto resultQueue = videoResults_.at(channel).get();
    auto frame = resultQueue->pop();
    dataReceiver_.notify(channel, frame);

    auto nextFrameNumber = frameInfo.frameNumber + PRELOAD_OFFSET;
    videoJobs_.push(VideoJob(channel, nextFrameNumber));
}


}
}

#endif