#ifndef AT_COMMAND_H
#define AT_COMMAND_H

#include <string>
#include <cstdint>

namespace xcs{
namespace xci{
namespace parrot{
    //! Possible states of the drone 'control' thread.
    enum ArdroneControlMode {
        STATE_NO_CONTROL_MODE = 0, /*<! Doing nothing */
        STATE_ARDRONE_UPDATE_CONTROL_MODE, /*<! Not used */
        STATE_PIC_UPDATE_CONTROL_MODE, /*<! Not used */
        STATE_LOGS_GET_CONTROL_MODE, /*<! Not used */
        STATE_CFG_GET_CONTROL_MODE, /*<! Send active configuration file to a client through the 'control' socket UDP 5559 */
        STATE_ACK_CONTROL_MODE, /*<! Reset command mask in navdata */
        STATE_CUSTOM_CFG_GET_CONTROL_MODE /*<! Requests the list of custom configuration IDs */
    };

    //! Basic behavior of parrot drone

    enum Behavior{
        STATE_TAKEOFF,
        STATE_LAND,
        STATE_EMERGENCY,
        STATE_NORMAL,
        STATE_CLEAR
    };

    //! Serve for easier conversion between 32-bit integer and float

    //! Use this union for preserving float values of an at command, because parrot
    //! expect this values written in at command as 32bit integer.

    union FloatInt {
        int32_t intNumber;
        float floatNumber;

        FloatInt(int32_t intNumber = 0) : intNumber(intNumber) {
        };

        FloatInt(float floatNumber) : floatNumber(floatNumber) {
        };
    };

    //! Drone's constant representation of movement. 

    //! All input value must be in range <-1,1> otherwise constructor and setter 
    //! correct input value to this range. All values lower or higher than limits
    //! will be replaced with this limits. 

    struct DroneMove {
        FloatInt roll; /*< Negative values makes the drone tilt to its left and positive to its right. */
        FloatInt pitch; /*< Negative values makes the drone lower its note and positive raise its nose. */
        FloatInt yaw; /*< Negative values makes the drone spin left and positive right. */
        FloatInt gaz; /*< Is percentage of the maximum vertical speed. Positive values makes the drone raise up and negative go down. */

        DroneMove(float roll = 0, float pitch = 0, float yaw = 0, float gaz = 0);
    };

    //! Basic class representing at command from which all at commands must be inherited.

    //! At command class serve only as general pointer to the inherited commands and printing.

    class AtCommand {
    public:
        //! Return text representation of at command.
        /*!
                \param sequenceNumber have to be in increasing order. Drone discard all at command with lower seqNumber than last one.
                \return text representation of an at command according to ar drone 2.0. documentation
                */
        virtual std::string toString(const int32_t sequenceNumber) = 0;
    };

    //! Drone take off, landing and emergency stop command.

    //! When you want change drone state from emergency to normal or otherwise
    //! first check in which state drone is because both commands have same 
    //! syntax. And when you send more then one command for changing drone state. 
    //! Drone will flip flop between this states.

    class AtCommandRef : public AtCommand {
        static const std::string nameOfCommand_; /*!< AT command name according to the ar drone 2.0 documentation. */
        Behavior basicBehavior_; /*< Represent behavior of drone (take off, land etc.).*/
    public:
        //! Initialize constant class representing at command for drone movement.

        AtCommandRef(const Behavior basicBehavior) : basicBehavior_(basicBehavior) {
        };
        std::string toString(const int32_t sequenceNumber);
    };

    //! Move drone command.

    class AtCommandPCMD : public AtCommand {
        static const std::string nameOfCommand_; /*!< AT command name according to the ar drone 2.0 documentation. */
        DroneMove move_;
        bool absoluteControl_;
        bool combinedYaw_;
        bool progressivCommand_;
    public:
        //! Initialize constant class representing at command for drone movement.
        /*!
        \param movement is state which drone will try accomplish. All values have to be in range <-1,1>.
        \param absControl set whether drone will use absolute control mode or not. Absolute control mode switch between controller frame and ar drone frame more in documentation.
        \param combYaw enable combined yaw control for generating complete turns (phi+yaw) from phi parameter.
        */
        AtCommandPCMD(const DroneMove movement, const bool absControl = false, const bool combYaw = false, const bool progCmd = false);
        std::string toString(const int32_t sequenceNumber);
    };

    //! Move drone command with absolute control support. 

    class AtCommandPCMD_MAG : public AtCommandPCMD {
        static const std::string nameOfCommand_; /*!< AT command name according to the ar drone 2.0 documentation. */
        FloatInt magnetoAngle_;
        FloatInt magnetoAccuracy_;
    public:
        //! Initialize constant class representing at command for drone movement.
        /*!
        \param movement is state which drone will try accomplish. All values have to be in range <-1,1>.
        \param absControl set whether drone will use absolute control mode or not. Absolute control mode switch between controller frame and ar drone frame more in documentation.
        \param combYaw enable combined yaw control for generating complete turns (phi+yaw) from phi parameter.
        \param psi is normalized angle <-1,1> from north provided by magnetometer sensor. Positive value means orientation to the east and negative to the west and 0 north.
        */
        AtCommandPCMD_MAG(const DroneMove movement, const float psi, const float psiAccur, const bool absControl = false, const bool combYaw = false, const bool progCmd = false);
        std::string toString(const int32_t sequenceNumber);
    };

    //! Sets the reference for horizontal plane. Must be on the ground.

    class AtCommandFTRIM : public AtCommand {
        static const std::string nameOfCommand_; /*!< AT command name according to the ar drone 2.0 documentation. */
    public:
        std::string toString(const int32_t sequenceNumber);
    };

    //! Configuration of the drone.

    class AtCommandCONFIG : public AtCommand {
        static const std::string nameOfCommand_; /*!< AT command name according to the ar drone 2.0 documentation. */
        std::string option_;
        std::string value_;
    public:
        //! Initialize at command with pair <option, value>. Class do not check if pair is valid!
        /*!
        \param option is name of ar drone parrot 2.0 option
        \param value is value of ar drone parrot 2.0 option
        */
        AtCommandCONFIG(const std::string option, const std::string value);
        std::string toString(const int32_t sequenceNumber);
    };

    //! Identifiers for atCommandConfig.

    class AtCommandCONFIG_IDS : public AtCommand {
        static const std::string nameOfCommand_; /*!< AT command name according to the ar drone 2.0 documentation. */
        std::string sessionID_;
        std::string userID_;
        std::string applicationID_;
    public:
        //! Identify to which session in the multi configuration setting will be assigned next AtCommandCONFIG. Have to be send before every AtCommandCONFIG in multi configuration setting.
        AtCommandCONFIG_IDS(const std::string sessionID, std::string userID, std::string applicationID);
        std::string toString(const int32_t sequenceNumber);
    };

    //! Reset the communication watchdog timer.

    class AtCommandCOMWDG : public AtCommand {
        static const std::string nameOfCommand_; /*!< AT command name according to the ar drone 2.0 documentation. */
    public:
        std::string toString(const int32_t sequenceNumber);
    };

    //! Ask the drone to calibrate the magnetometer. Must be flying.

    class AtCommandCALIB : public AtCommand {
        static const std::string nameOfCommand_; /*!< AT command name according to the ar drone 2.0 documentation. */
        int device_;
    public:
        //! Set which device will calibrate magnetometer.
        /*!
        \param device is identifier of the device to calibrate. Choose this identifier from ardrone_calibration_device_t.
        */
        AtCommandCALIB(const int device);
        std::string toString(const int32_t sequenceNumber);
    };

    //! 

    class AtCommandCTRL : public AtCommand {
        static const std::string nameOfCommand_; /*!< AT command name according to the ar drone 2.0 documentation. */
        ArdroneControlMode mode_;
    public:

        AtCommandCTRL(ArdroneControlMode mode = STATE_NO_CONTROL_MODE) : mode_(mode) {
        };
        std::string toString(const int32_t sequenceNumber);
    };
}}}
#endif
