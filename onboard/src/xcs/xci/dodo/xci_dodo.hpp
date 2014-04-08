#ifndef XCI_DODO_H
#define XCI_DODO_H

#include <xcs/xci/xci.hpp>
#include <xcs/types/cartesian_vector.hpp>
#include <xcs/types/eulerian_vector.hpp>

#include <cstddef>
#include <cstdint>
#include <cmath>
#include <cstdlib>
#include <thread>

#include "basic.hpp"
#include "video_player.hpp"


namespace xcs {
namespace xci {
namespace dodo {

/*!
 * Simple XCI implementation for debugging and testing.
 * 
 * Available configuration:
 *     video:filename -- path to video file
 *     video:fps      -- approximate FPS of the video
 *     log:fp         -- 0|1 whether to log fly params
 *     log:command    -- 0|1 whether to log fly commands
 * 
 * Special commands
 *     Load    loads video file
 *     Play    starts playing video file
 *     Pause   pauses playing (no video frames are notified)
 *     Stop    pause playing and rewinds video file
 * 
 */
class XciDodo : public virtual XCI {
public:
    XciDodo(DataReceiver& dataReceiver);
    ~XciDodo();

    virtual std::string name();

    //! A pure virtual member returning list of available sensors on x-copter
    virtual SensorList sensorList();

    //! A pure virtual member returning specific x-copter's parameter.
    virtual ParameterValueType parameter(ParameterNameType name);

    //! A pure virtual member returning x-copter's configuration value
    virtual std::string configuration(const std::string &key);

    //! A pure virtual member returning x-copter's configuration values
    virtual InformationMap configuration();

    //! A pure virtual member returning list of x-copter's special commands 
    virtual SpecialCMDList specialCMD();

    //! A pure virtual member sets configuration value.
    virtual void configuration(const std::string &key, const std::string &value);

    //! A pure virtual member taking new x-copter's configuration and send this configuration to the x-copter
    virtual void configuration(const InformationMap &configuration);

    //! A pure virtual member taking command from list of x-copter's special commands and sending it to the x-copter
    virtual void command(const std::string &command);

    //! A pure virtual member taking four fly parameters and send it to the x-copter
    virtual void flyParam(float roll, float pitch, float yaw, float gaz);

    //! A pure virtual member initializing XCI for use
    virtual void init();

    //! Resets settings to default values and re-calibrates the sensors (if supported).
    virtual void reset();

    //! Turns on the engines.
    virtual void start();

    //! Turns off the engines.
    virtual void stop();
private:

    enum VideoStatus {
        VIDEO_UNLOADED, //! No file was loaded
        VIDEO_PAUSED, //! File loaded, no frames being rendered
        VIDEO_PLAYING //! File loaded, render frames
    };
    static const std::string NAME_;


    //! How often alive signal is sent (in Hz) (must divisor of 1000/SENSOR_PERIOD_).
    static const size_t ALIVE_FREQ_;
    //! How often navdata data is sent (in Hz) (must divisor of 1000/SENSOR_PERIOD_).
    static const size_t NAVDATA_FREQ_;
    //! Period of sensor thread in ms.
    static const size_t SENSOR_PERIOD_;
    //! Milliseconds for video thread when not playing
    static const size_t VIDEO_IDLE_SLEEP_;

    static const std::string CMD_VIDEO_LOAD_;
    static const std::string CMD_VIDEO_PLAY_;
    static const std::string CMD_VIDEO_PAUSE_;
    static const std::string CMD_VIDEO_STOP_;
    static const std::string CMD_VIDEO_STEP_;

    static const SpecialCMDList specialCommands_;

    bool inited_;
    std::thread sensorThread_;
    std::thread videoThread_;

    //! Video FPS
    size_t videoFps_;

    VideoPlayer videoPlayer_;

    VideoStatus videoStatus_;

    static const std::string CONFIG_VIDEO_FILENAME;
    static const std::string CONFIG_VIDEO_FPS;
    static const std::string CONFIG_VIDEO_FONT;
    static const std::string CONFIG_VIDEO_TIMESTAMPS;
    static const std::string CONFIG_LOG_FP;
    static const std::string CONFIG_LOG_COMMAND;

    InformationMap configuration_;

    /*! Pseudophysical state */

    xcs::CartesianVector position_;
    xcs::CartesianVector velocity_;
    xcs::EulerianVector rotation_;
    
    //! Generates dummy data for sensors
    void sensorGenerator();

    //! Generates frames
    void videoPlayer();

    void renderFrame();

};
}
}
}
#endif