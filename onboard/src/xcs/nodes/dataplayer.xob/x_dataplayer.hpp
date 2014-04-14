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
    PLAYBACK_SKIPPED // spede scaling is done by skipping samples
};
};

class XDataplayer : public xcs::nodes::XObject {
    PlaybackMode playbackMode_;
    double playbackSpeed_;
    
    void playbackSpeed(double value);
    void playbackMode(PlaybackMode value);
    
public:
    typedef uint64_t TimestampType;
    
    XDataplayer(const std::string& name);
    ~XDataplayer();

    void init(const std::string &file);
    
    urbi::UVar playbackModeUVar;
    urbi::UVar playbackSpeedUVar;

    void playbackPlay();
    void playbackStop();
    void playbackPause();
    void playbackTo(TimestampType timestamp);

    
};

}
}

#endif