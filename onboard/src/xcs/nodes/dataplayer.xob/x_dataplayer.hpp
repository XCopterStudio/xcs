#ifndef DATALOGGER_H
#define DATALOGGER_H

#include <string>
#include <fstream>
#include <mutex>
#include <cstdint>

#include <xcs/nodes/xobject/x_object.hpp>

namespace xcs {
namespace nodes {

namespace dataplayer {

enum PlaybackMode {
    PLAYBACK_SMOOTH //! speed scaling is done smoothly,
    PLAYBACK_SKIPPED // speed scaling is done by skipping samples
};
};

class XDataplayer : public xcs::nodes::XObject {
    PlaybackMode playbackMode_;
    double playbackSpeed_;

    void playbackSpeed(double value);
    void playbackMode(PlaybackMode value);

    void playbackPlay();
    void playbackStop();
    void playbackPause();
    void playbackSeek(TimestampType timestamp);
    
    void playerLoop();
    
    void loadHeader();
    
    std::ifstream file_;
public:
    typedef uint64_t TimestampType;

    XDataplayer(const std::string& name);
    ~XDataplayer();

    void init(const std::string &file);

    urbi::UVar playbackModeUVar;
    urbi::UVar playbackSpeedUVar;

    xcs::nodes::XInputPort<std::string> command;
    xcs::nodes::XInputPort<TimestampType> seek;
};

}
}

#endif