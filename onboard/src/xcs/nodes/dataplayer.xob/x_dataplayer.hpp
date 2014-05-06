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
#include <xcs/xci/data_receiver.hpp>

namespace xcs {
namespace nodes {

namespace dataplayer {

enum PlaybackMode {
    PLAYBACK_SMOOTH, //! speed scaling is done smoothly,
    PLAYBACK_SKIPPED // speed scaling is done by skipping samples
};
};

class XDataplayer : public xcs::nodes::XObject {
    typedef uint64_t TimestampType;

public:

    XDataplayer(const std::string& name);
    ~XDataplayer();

    urbi::UVar playbackModeUVar;
    urbi::UVar playbackSpeedUVar;

    xcs::nodes::XInputPort<std::string> command;
    xcs::nodes::XInputPort<TimestampType> seek;


    void init(const std::string &file);
private:
    const static std::string CMD_PLAY;
    const static std::string CMD_PAUSE;

    dataplayer::PlaybackMode playbackMode_;
    double playbackSpeed_;
    xci::DataReceiver dataReceiver_;
    std::ifstream file_;
    std::atomic<bool> isPlaying_;
    std::thread dataLoopThread_;
    
    void playbackSpeed(double value);
    void playbackMode(dataplayer::PlaybackMode value);

    void playbackPlay();
    void playbackStop();
    void playbackPause();
    void playbackSeek(TimestampType timestamp);

    void loop();

    void loadHeader();

    void onCommand(const std::string &command);
};

}
}

#endif