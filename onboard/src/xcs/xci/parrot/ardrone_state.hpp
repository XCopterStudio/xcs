#ifndef ARDRONE_STATE_H
#define ARDRONE_STATE_H

#include <cstdint>
#include <mutex>

namespace xcs{
namespace xci{
namespace parrot{

    enum ArdronePossibleStates{
        FLAG_ARDRONE_FLY_MASK = 1U << 0,  /*!< FLY MASK : (0) ardrone is landed, (1) ardrone is flying */
        FLAG_ARDRONE_VIDEO_MASK = 1U << 1,  /*!< VIDEO MASK : (0) video disable, (1) video enable */
        FLAG_ARDRONE_VISION_MASK = 1U << 2,  /*!< VISION MASK : (0) vision disable, (1) vision enable */
        FLAG_ARDRONE_CONTROL_MASK = 1U << 3,  /*!< CONTROL ALGO : (0) euler angles control, (1) angular speed control */
        FLAG_ARDRONE_ALTITUDE_MASK = 1U << 4,  /*!< ALTITUDE CONTROL ALGO : (0) altitude control inactive (1) altitude control active */
        FLAG_ARDRONE_USER_FEEDBACK_START = 1U << 5,  /*!< USER feedback : Start button state */
        FLAG_ARDRONE_COMMAND_MASK = 1U << 6,  /*!< Control command ACK : (0) None, (1) one received */
        FLAG_ARDRONE_CAMERA_MASK = 1U << 7,  /*!< CAMERA MASK : (0) camera not ready, (1) Camera ready */
        FLAG_ARDRONE_TRAVELLING_MASK = 1U << 8,  /*!< Travelling mask : (0) disable, (1) enable */
        FLAG_ARDRONE_USB_MASK = 1U << 9,  /*!< USB key : (0) usb key not ready, (1) usb key ready */
        FLAG_ARDRONE_NAVDATA_DEMO_MASK = 1U << 10, /*!< Navdata demo : (0) All navdata, (1) only navdata demo */
        FLAG_ARDRONE_NAVDATA_BOOTSTRAP = 1U << 11, /*!< Navdata bootstrap : (0) options sent in all or demo mode, (1) no navdata options sent */
        FLAG_ARDRONE_MOTORS_MASK = 1U << 12, /*!< Motors status : (0) Ok, (1) Motors problem */
        FLAG_ARDRONE_COM_LOST_MASK = 1U << 13, /*!< Communication Lost : (1) com problem, (0) Com is ok */
        FLAG_ARDRONE_SOFTWARE_FAULT = 1U << 14, /*!< Software fault detected - user should land as quick as possible (1) */
        FLAG_ARDRONE_VBAT_LOW = 1U << 15, /*!< VBat low : (1) too low, (0) Ok */
        FLAG_ARDRONE_USER_EL = 1U << 16, /*!< User Emergency Landing : (1) User EL is ON, (0) User EL is OFF*/
        FLAG_ARDRONE_TIMER_ELAPSED = 1U << 17, /*!< Timer elapsed : (1) elapsed, (0) not elapsed */
        FLAG_ARDRONE_MAGNETO_NEEDS_CALIB = 1U << 18, /*!< Magnetometer calibration state : (0) Ok, no calibration needed, (1) not ok, calibration needed */
        FLAG_ARDRONE_ANGLES_OUT_OF_RANGE = 1U << 19, /*!< Angles : (0) Ok, (1) out of range */
        FLAG_ARDRONE_WIND_MASK = 1U << 20, /*!< WIND MASK: (0) ok, (1) Too much wind */
        FLAG_ARDRONE_ULTRASOUND_MASK = 1U << 21, /*!< Ultrasonic sensor : (0) Ok, (1) deaf */
        FLAG_ARDRONE_CUTOUT_MASK = 1U << 22, /*!< Cutout system detection : (0) Not detected, (1) detected */
        FLAG_ARDRONE_PIC_VERSION_MASK = 1U << 23, /*!< PIC Version number OK : (0) a bad version number, (1) version number is OK */
        FLAG_ARDRONE_ATCODEC_THREAD_ON = 1U << 24, /*!< ATCodec thread ON : (0) thread OFF (1) thread ON */
        FLAG_ARDRONE_NAVDATA_THREAD_ON = 1U << 25, /*!< Navdata thread ON : (0) thread OFF (1) thread ON */
        FLAG_ARDRONE_VIDEO_THREAD_ON = 1U << 26, /*!< Video thread ON : (0) thread OFF (1) thread ON */
        FLAG_ARDRONE_ACQ_THREAD_ON = 1U << 27, /*!< Acquisition thread ON : (0) thread OFF (1) thread ON */
        FLAG_ARDRONE_CTRL_WATCHDOG_MASK = 1U << 28, /*!< CTRL watchdog : (1) delay in control execution (> 5ms), (0) control is well scheduled */
        FLAG_ARDRONE_ADC_WATCHDOG_MASK = 1U << 29, /*!< ADC Watchdog : (1) delay in uart2 dsr (> 5ms), (0) uart2 is good */
        FLAG_ARDRONE_COM_WATCHDOG_MASK = 1U << 30, /*!< Communication Watchdog : (1) com problem, (0) Com is ok */
        #if defined(__ARMCC_VERSION)
            FLAG_ARDRONE_EMERGENCY_MASK      = (int)0x80000000  /*!< Emergency landing : (0) no emergency, (1) emergency */
        #else
            FLAG_ARDRONE_EMERGENCY_MASK = 1U << 31  /*!< Emergency landing : (0) no emergency, (1) emergency */
        #endif
    };

    class ArdroneState{
        uint32_t bitFieldStates_;
        std::mutex stateMutex_;
    public:
        ArdroneState() : bitFieldStates_(0) {};
        void updateState(uint32_t bitFieldStates){ std::lock_guard<std::mutex> lock(stateMutex_); this->bitFieldStates_ = bitFieldStates; };
        bool getState(ArdronePossibleStates state){ std::lock_guard<std::mutex> lock(stateMutex_); return state & bitFieldStates_ ? true : false; };
    };

}}}
#endif