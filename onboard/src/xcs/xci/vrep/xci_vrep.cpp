#include "xci_vrep.hpp"

#include <xcs/xcs_fce.hpp>
#include <xcs/types/cartesian_vector.hpp>
#include <xcs/types/eulerian_vector.hpp>
#include <xcs/xci/data_receiver.hpp>

using namespace xcs;
using namespace xcs::xci;
using namespace xcs::xci::vrep;

const float XciVrep::POS_MULTI = 2;

void XciVrep::updateSensors(){
    while (!endAll_){
        if (simxGetConnectionId(clientID_)!=-1){ // we have connection with simulation server
            // get drone position
            float vector[3];
            float angle[3];
            if (simxGetObjectPosition(clientID_, droneHandler_, -1, vector, simx_opmode_buffer) == simx_return_ok){
                dataReceiver_.notify("position", CartesianVector(vector[0], vector[1], vector[2]));
                dronePosition_.x = vector[0];
                dronePosition_.y = vector[1];
                dronePosition_.z = vector[2];
                //printf("Drone position [%f,%f,%f] \n", vector[0], vector[1], vector[2]);
            }

            if (simxGetObjectVelocity(clientID_, droneHandler_, vector, angle, simx_opmode_buffer) == simx_return_ok){
                dataReceiver_.notify("velocity", CartesianVector(vector[0], vector[1], vector[2]));
            }

            if (simxGetObjectOrientation(clientID_, droneHandler_, droneHandler_, angle, simx_opmode_buffer) == simx_return_ok){
                dataReceiver_.notify("rotation", EulerianVector(angle[0], angle[1], angle[2]));
                droneRotation_.phi = normAngle(degreesToRadians(angle[0]));
                droneRotation_.theta = normAngle(degreesToRadians(angle[1]));
                droneRotation_.psi = normAngle(degreesToRadians(angle[2]));
                printf("XciVrep: drone rotation[%f,%f,%f] \n", droneRotation_.phi, droneRotation_.theta, droneRotation_.psi);
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

// ================ public functions ================

XciVrep::XciVrep(DataReceiver& dataReceiver, std::string droneName, std::string targetName, std::string address, int portNumber) :
XCI(dataReceiver),
droneName_(droneName),
targetName_(targetName),
address_(address),
portNumber_(portNumber)
{
    clientID_ = -1;
    endAll_ = false;
    configuration_["XCI_PARAM_FP_PERSISTENCE"] = "50";
}

XciVrep::~XciVrep(){
    if (clientID_ != -1){
        simxFinish(clientID_);
    }

    endAll_ = true;
}

std::string XciVrep::name(){
    return "xci_vrep";
}

SensorList XciVrep::sensorList(){
    SensorList sensorList;

    sensorList.push_back(Sensor("position", "POSITION_ABS"));
    sensorList.push_back(Sensor("velocity", "VELOCITY_ABS"));
    sensorList.push_back(Sensor("rotation", "ROTATION"));
    
    return sensorList;
}

std::string XciVrep::configuration(const std::string &key){
    if (configuration_.count(key) > 0){
        return configuration_[key];
    }
    else{
        return "";
    }
}

InformationMap XciVrep::configuration(){
    return configuration_;
}

SpecialCMDList XciVrep::specialCMD(){
    return SpecialCMDList();
}

void XciVrep::configuration(const std::string &key, const std::string &value){}

void XciVrep::configuration(const InformationMap &configuration){}

void XciVrep::command(const std::string &command){}

void XciVrep::flyControl(float roll, float pitch, float yaw, float gaz){
    printf("XciVrep: flyControl [%f,%f,%f]\n", roll, pitch, yaw);
    if (simxGetConnectionId(clientID_) != -1){ // we have connection with simulation server
        float angle[3];
        angle[0] = 0;
        angle[1] = 0;
        angle[2] = radiansToDegrees(normAngle(droneRotation_.psi + yaw));
        simxSetObjectOrientation(clientID_, targetHandler_, -1, angle, simx_opmode_oneshot);
        //printf("XciVrep: New rotation [%f,%f,%f] \n", angle[0], angle[1], angle[2]);
        // gaz    
        float position[3];
        position[0] = dronePosition_.x + cos(droneRotation_.psi)*roll*POS_MULTI - sin(droneRotation_.psi)*pitch*POS_MULTI;
        position[1] = dronePosition_.y - sin(droneRotation_.psi)*roll*POS_MULTI - cos(droneRotation_.psi)*pitch*POS_MULTI;
        position[2] = dronePosition_.z + gaz;
        simxSetObjectPosition(clientID_, targetHandler_, -1, position, simx_opmode_oneshot);
        //printf("XciVrep: New position [%f,%f,%f] \n", position[0],position[1],position[2]);
    }
}

void XciVrep::init(){
    // connect to the remote simulator with reconnection
    clientID_ = simxStart(address_.c_str(), portNumber_, true, false, 2000, 5); 
    if (clientID_ != -1){
        simxGetObjectHandle(clientID_, droneName_.c_str(), &droneHandler_, simx_opmode_oneshot_wait);
        simxGetObjectHandle(clientID_, targetName_.c_str(), &targetHandler_, simx_opmode_oneshot_wait);

        if (droneHandler_ >= 0 && targetHandler_ >= 0){
            // Send request for periodic updates
            simxGetObjectPosition(clientID_, droneHandler_, -1, NULL,simx_opmode_streaming);
            simxGetObjectVelocity(clientID_, droneHandler_, NULL, NULL, simx_opmode_streaming);
            simxGetObjectOrientation(clientID_, droneHandler_, -1, NULL, simx_opmode_streaming);

            updateThread_ = std::thread(&XciVrep::updateSensors, this);
        }
        else{
            // TODO: error
        }
    }else{
        // TODO: error can not connect with simulation
    }
}

extern "C" {

    XCI* CreateXci(DataReceiver &dataReceiver) {
        return new XciVrep(dataReceiver);
    }
}