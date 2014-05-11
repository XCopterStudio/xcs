#include "xci_vrep.hpp"

#include <xcs/types/cartesian_vector.hpp>
#include <xcs/types/eulerian_vector.hpp>
#include <xcs/xci/data_receiver.hpp>

using namespace xcs;
using namespace xcs::xci;
using namespace xcs::xci::vrep;

const float XciVrep::MAX_ANGLE = M_PI_4;

void XciVrep::updateSensors(){
    while (!endAll_){
        if (simxGetConnectionId(clientID_)!=-1){ // we have connection with simulation server
            // get drone position
            float vector[3];
            float angle[3];
            if (simxGetObjectPosition(clientID_, droneHandler_, -1, vector, simx_opmode_buffer) == simx_return_ok){
                dataReceiver_.notify("position", CartesianVector(vector[0], vector[1], vector[2]));
                printf("Drone position [%f,%f,%f] \n", vector[0], vector[1], vector[2]);
            }

            if (simxGetObjectVelocity(clientID_, droneHandler_, vector, angle, simx_opmode_buffer) == simx_return_ok){
                dataReceiver_.notify("velocity", CartesianVector(vector[0], vector[1], vector[2]));
            }

            if (simxGetObjectOrientation(clientID_, droneHandler_, droneHandler_, angle, simx_opmode_buffer) == simx_return_ok){
                dataReceiver_.notify("rotation", EulerianVector(angle[0], angle[1], angle[2]));
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

// ================ public functions ================

XciVrep::XciVrep(DataReceiver& dataReceiver, std::string name, std::string address, int portNumber) :
XCI(dataReceiver),
name_(name),
address_(address),
portNumber_(portNumber)
{
    clientID_ = -1;
    endAll_ = false;
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

ParameterValueType XciVrep::parameter(ParameterNameType name){
    switch (name) {
    case XCI_PARAM_FP_PERSISTENCE:
        return "50";
    default:
        throw std::runtime_error("Parameter not defined.");
    }
}

std::string XciVrep::configuration(const std::string &key){
    return "";
}

InformationMap XciVrep::configuration(){
    return InformationMap();
}

SpecialCMDList XciVrep::specialCMD(){
    return SpecialCMDList();
}

void XciVrep::configuration(const std::string &key, const std::string &value){}

void XciVrep::configuration(const InformationMap &configuration){}

void XciVrep::command(const std::string &command){}

void XciVrep::flyControl(float roll, float pitch, float yaw, float gaz){
    if (simxGetConnectionId(clientID_) != -1){ // we have connection with simulation server
        float angle[3];
        angle[0] = roll * MAX_ANGLE;
        angle[1] = pitch * MAX_ANGLE;
        angle[2] = yaw * MAX_ANGLE;
        simxSetObjectOrientation(clientID_, droneHandler_, droneHandler_, angle, simx_opmode_oneshot);

        // TODO: implement gaz
    }
}

void XciVrep::init(){
    // connect to the remote simulator with reconnection
    clientID_ = simxStart(address_.c_str(), portNumber_, true, false, 2000, 5); 
    if (clientID_ != -1){
        simxGetObjectHandle(clientID_, name_.c_str(), &droneHandler_, simx_opmode_oneshot_wait);

        if (droneHandler_ >= 0){
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

void XciVrep::reset(){}

void XciVrep::start(){}

void XciVrep::stop(){}

extern "C" {

    XCI* CreateXci(DataReceiver &dataReceiver) {
        return new XciVrep(dataReceiver);
    }
}