#ifndef XCI_DODO_H
#define XCI_DODO_H

#include <xcs/xci/xci.hpp>

#include <cstddef>
#include <cstdint>
#include <cmath>
#include <cstdlib>
#include <thread>

#include "basic.hpp"


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
    static const size_t VIDEO_FPS_ = 10;
    //! Random disturbances on each pixel from [0, 255]
    static const size_t VIDEO_NOISE_ = 64;
    static uint8_t frames_[VIDEO_HEIGHT_][VIDEO_WIDTH_][VIDEO_COLORS_];

    //! How often alive signal is sent (in Hz) (must divisor of 1000/SENSOR_PERIOD_).
    static const size_t ALIVE_FREQ_;
    //! Period of sensor thread in ms.
    static const size_t SENSOR_PERIOD_;

    bool inited_;
    std::thread sensorThread_;

    //! Generates dummy data for sensors
    void sensorGenerator();

    /*!
     * Fills buffer with computed video frame.
     * It's templated via noise parameter becuasue with noise == 0, compiler might optimize the rand call.
     */
    template<int16_t noise>
    void renderFrame(size_t frameNo) {
        static const size_t width = 10;
        static const size_t amplitude = 80;
        static const double speed = 0.2;


        for (auto y = 0; y < VIDEO_HEIGHT_; ++y) {
            size_t seed = rand();
            auto linepos = (VIDEO_WIDTH_ / 2) + static_cast<size_t> (amplitude * sin(frameNo * speed));
            for (auto x = 0; x < VIDEO_WIDTH_; ++x) {
                int16_t color = (x >= linepos && x < linepos + width) ? 0 : 255;
                if (noise > 0) {
                    //color += rand() % (2 * noise) - noise;
                    // calling rand for each pixel is surprisingly CPU expensive, so we use this hand-made low-entropy random number generator
                    color += ((x * frameNo * seed + x * linepos * y + frameNo) % 7919) % (2 * noise) - noise;
                }
                frames_[y][x][0] = static_cast<uint8_t> (valueInRange<int16_t>(color, 0, 255));
                frames_[y][x][1] = static_cast<uint8_t> (valueInRange<int16_t>(color, 0, 255));
                frames_[y][x][2] = static_cast<uint8_t> (valueInRange<int16_t>(color, 0, 255));
            }
        }
    }

};
}
}
}
#endif