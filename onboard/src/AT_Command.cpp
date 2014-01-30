#include "AT_Command.hpp"
#include "../../shared/basic.hpp"

#include <sstream>

//! General constants for creating at commands.
static const std::string startOfAtComm = "AT*";
static const std::string endOfAtComm = "\r";
static const std::string delOfAtCommVal = ",";

//! Specific names for all at commands
const std::string AtCommandRef::nameOfCommand_ = "REF";
const std::string AtCommandPCMD::nameOfCommand_ = "PCMD";
const std::string AtCommandPCMD_MAG::nameOfCommand_ = "PCMD_MAG";
const std::string AtCommandFTRIM::nameOfCommand_ = "FTRIM";
const std::string AtCommandCALIB::nameOfCommand_ = "CALIB";
const std::string AtCommandCONFIG::nameOfCommand_ = "CONFIG";
const std::string AtCommandCONFIG_IDS::nameOfCommand_ = "CONFIG_IDS";
const std::string AtCommandCOMWDG::nameOfCommand_ = "COMWDG";
const std::string AtCommandCTRL::nameOfCommand_ = "CTRL";

DroneMove::DroneMove(float roll, float pitch, float yaw, float gaz) {
    this->roll.floatNumber = valueInRange(roll, 1.0f);
    this->pitch.floatNumber = valueInRange(pitch, 1.0f);
    this->yaw.floatNumber = valueInRange(yaw, 1.0f);
    this->gaz.floatNumber = valueInRange(gaz, 1.0f);
}

std::string AtCommandRef::toString(const int32_t sequenceNumber) {
    // create magic bitField for basic behavior (take off, land, etc.)
    // from documentation this bits must be set to 1
    int32_t bitField = (1 << 18) | (1 << 20) | (1 << 22) | (1 << 24) | (1 << 28);
    switch (basicBehavior_) {
    case STATE_TAKEOFF:
        bitField |= (1 << 9); // 9-bit of bit field decides between take-off and land, 1 for take off, 0 for land
        break;
    case STATE_LAND:
        bitField &= ~(1 << 9); // 9-bit of bit field decides between take-off and land, 1 for take off, 0 for land
        break;
    case STATE_EMERGENCY:
        bitField |= (1 << 8); // when drone is in normal state this bit set to 1 makes the drones enter to an emergency mode (cut off engine).
        break;
    case STATE_NORMAL:
        bitField |= (1 << 8); // when drone is in emergency state this bit set to 1 makes the drones resume a normal mode.
    case STATE_CLEAR:
        // don't change anything on bit field
        break;
    default: // this state is not accessible
        break;
    }

    std::stringstream out;
    out << startOfAtComm << nameOfCommand_ << "=" << sequenceNumber;
    out << delOfAtCommVal;
    out << bitField;
    out << endOfAtComm;

    return out.str();
}

AtCommandPCMD::AtCommandPCMD(const DroneMove movement, const bool absControl, const bool combYaw, const bool progCmd) : move_(movement), absoluteControl_(absControl), combinedYaw_(combYaw), progressivCommand_(progCmd) {

}

std::string AtCommandPCMD::toString(const int32_t sequenceNumber) {
    int32_t bitField = 0;
    // Create bitField according to Boolean arguments. More details in ar drone documentation.
    bitField |= (progressivCommand_ << 0);
    bitField |= (combinedYaw_ << 1);
    bitField |= (absoluteControl_ << 2);

    std::stringstream out;
    out << startOfAtComm << nameOfCommand_ << "=" << sequenceNumber;
    out << delOfAtCommVal << bitField;
    out << delOfAtCommVal << move_.roll.intNumber;
    out << delOfAtCommVal << move_.pitch.intNumber;
    out << delOfAtCommVal << move_.gaz.intNumber;
    out << delOfAtCommVal << move_.yaw.intNumber;
    out << endOfAtComm;

    return out.str();
}

AtCommandPCMD_MAG::AtCommandPCMD_MAG(const DroneMove movement, const float psi, const float psiAccur, const bool absControl, const bool combYaw, const bool progCmd) : AtCommandPCMD(movement) {
    magnetoAngle_ = valueInRange(psi, 1.0f);
    magnetoAccuracy_ = valueInRange(psiAccur, 1.0f);
}

std::string AtCommandPCMD_MAG::toString(const int32_t sequenceNumber) {
    std::string pcmd = AtCommandPCMD::toString(sequenceNumber);

    std::stringstream out;
    // remove last character (\r) from pcmd command
    out << pcmd.substr(0, pcmd.length() - 1);
    // append additional values of drone heading according to magnetometer 
    out << delOfAtCommVal << magnetoAngle_.intNumber;
    out << delOfAtCommVal << magnetoAccuracy_.intNumber;
    out << endOfAtComm;

    return out.str();
}

std::string AtCommandFTRIM::toString(const int32_t sequenceNumber) {
    std::stringstream out;
    out << startOfAtComm << nameOfCommand_ << "=" << sequenceNumber << endOfAtComm;

    return out.str();
}

AtCommandCONFIG::AtCommandCONFIG(const std::string option, const std::string value) : option_(option), value_(value) {

}

std::string AtCommandCONFIG::toString(const int32_t sequenceNumber) {
    std::stringstream out;
    out << startOfAtComm << nameOfCommand_ << "=" << sequenceNumber;
    out << delOfAtCommVal << "\"" << option_ << "\"";
    out << delOfAtCommVal << "\"" << value_ << "\"";
    out << endOfAtComm;

    return out.str();
}

AtCommandCONFIG_IDS::AtCommandCONFIG_IDS(const std::string sessionID, std::string userID, std::string applicationID) : sessionID_(sessionID), userID_(userID), applicationID_(applicationID) {

}

std::string AtCommandCONFIG_IDS::toString(const int32_t sequenceNumber) {
    std::stringstream out;
    out << startOfAtComm << nameOfCommand_ << "=" << sequenceNumber;
    out << delOfAtCommVal << "\"" << sessionID_ << "\"";
    out << delOfAtCommVal << "\"" << userID_ << "\"";
    out << delOfAtCommVal << "\"" << applicationID_ << "\"";
    out << endOfAtComm;

    return out.str();
}

std::string AtCommandCOMWDG::toString(const int32_t sequenceNumber) {
    std::stringstream out;
    out << startOfAtComm << nameOfCommand_ << "=" << sequenceNumber << endOfAtComm;

    return out.str();
}

AtCommandCALIB::AtCommandCALIB(const int device) : device_(device) {

}

std::string AtCommandCALIB::toString(const int32_t sequenceNumber) {
    std::stringstream out;
    out << startOfAtComm << nameOfCommand_ << "=" << sequenceNumber;
    out << delOfAtCommVal << device_;
    out << endOfAtComm;
    return out.str();
}

std::string AtCommandCTRL::toString(const int32_t sequenceNumber) {
    std::stringstream out;
    out << startOfAtComm << nameOfCommand_ << "=" << sequenceNumber;
    out << delOfAtCommVal << mode_;
    out << delOfAtCommVal << (int32_t)0;
    out << endOfAtComm;
    return out.str();
}


