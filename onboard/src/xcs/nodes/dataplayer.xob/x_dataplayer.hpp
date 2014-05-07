#ifndef DATALOGGER_H
#define DATALOGGER_H

#include <string>
#include <fstream>
#include <mutex>
#include <thread>
#include <cstdint>
#include <atomic>

#include <xcs/nodes/xobject/x_object.hpp>
#include <xcs/nodes/xobject/x_input_port.hpp>
#include <xcs/types/timestamp.hpp>
#include <xcs/types/type_utils.hpp>
#include <xcs/xci/data_receiver.hpp>

#include "video_player.hpp"


namespace xcs {
namespace nodes {

namespace dataplayer {

enum PlaybackMode {
    PLAYBACK_SMOOTH, //! speed scaling is done smoothly,
    PLAYBACK_SKIPPED // speed scaling is done by skipping samples
};
};

class XDataplayer : public xcs::nodes::XObject {
public:

    XDataplayer(const std::string& name);
    ~XDataplayer();

    urbi::UVar playbackModeUVar;
    urbi::UVar playbackSpeedUVar;

    xcs::nodes::XInputPort<std::string> command;
    xcs::nodes::XInputPort<xcs::Timestamp> seek;


    void init(const std::string &file);
private:
    typedef std::map<std::string, std::string> SyntacticMap;

    const static std::string CMD_PLAY;
    const static std::string CMD_PAUSE;

    static xcs::SyntacticCategoryMap syntacticCategoryMap_;

    SyntacticMap channelTypes_;

    dataplayer::PlaybackMode playbackMode_;
    double playbackSpeed_;
    xci::DataReceiver dataReceiver_;
    std::ifstream file_;
    std::atomic<bool> isPlaying_;
    std::thread dataLoopThread_;
    std::thread videoDecodeThread_;

    void playbackSpeed(double value);
    void playbackMode(dataplayer::PlaybackMode value);

    void playbackPlay();
    void playbackStop();
    void playbackPause();
    //void playbackSeek(xcs::TimestampType timestamp);

    void loadHeader();

    void processHeaderLine(const std::string &line);

    void loop();
    
    void processLogLine(const std::string &channel, const Timestamp timestamp);

    void onCommand(const std::string &command);

    template<typename T>
    T getFrame(FrameInfo frameInfo);

};

template<>
BitmapType XDataplayer::getFrame<BitmapType>(FrameInfo frameInfo) {
    return BitmapType();
}

template<>
BitmapTypeChronologic XDataplayer::getFrame<BitmapTypeChronologic>(FrameInfo frameInfo) {
    return BitmapTypeChronologic();
}


}
}

#endif