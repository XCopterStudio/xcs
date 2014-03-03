#ifndef XCI_DODO_H
#define XCI_DODO_H

#include <xcs/xci/xci.hpp>

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
    static const std::string NAME_;

    static const size_t VIDEO_WIDTH_ = 640;
    static const size_t VIDEO_HEIGHT_ = 480;
    static const size_t VIDEO_COLORS_ = 3;
    //! Video FPS (must divisor of 1000/SENSOR_PERIOD_).
    static const size_t VIDEO_FPS_ = 25;
    //! Random disturbances on each pixel from [0, 255]
    static const size_t VIDEO_NOISE_ = 64;
    static uint8_t frames_[VIDEO_HEIGHT_][VIDEO_WIDTH_][VIDEO_COLORS_];

    //! How often alive signal is sent (in Hz) (must divisor of 1000/SENSOR_PERIOD_).
    static const size_t ALIVE_FREQ_;
    //! Period of sensor thread in ms.
    static const size_t SENSOR_PERIOD_;

    bool inited_;
    std::thread sensorThread_;

    VideoPlayer videoPlayer_;

    //! Generates dummy data for sensors
    void sensorGenerator();

    /*!
     * Fills buffer with computed video frame.     * 
     */
    void renderFrame(size_t frameNo, int16_t noise);


};
}
}
}
#endif