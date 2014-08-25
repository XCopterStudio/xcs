#include "xci_vrep.hpp"

#include <xcs/xcs_fce.hpp>
#include <xcs/types/cartesian_vector.hpp>
#include <xcs/types/eulerian_vector.hpp>
#include <xcs/xci/data_receiver.hpp>

#include <xcs/logging.hpp>

using namespace xcs;
using namespace xcs::xci;
using namespace xcs::xci::vrep;

const float XciVrep::POS_MULTI = 10;
const unsigned int XciVrep::ATTEMPT_COUNT = 5;

void XciVrep::updateSensors() {
    while (!endAll_) {
        if (simxGetConnectionId(clientID_) != -1) { // we have connection with simulation server
            // get drone position
            float vector[3];
            float angle[3];
            if (simxGetObjectPosition(clientID_, droneHandler_, -1, vector, simx_opmode_buffer) == simx_return_ok) {
                dataReceiver_.notify("position", CartesianVector(vector[0], vector[1], vector[2]));
                dronePosition_.x = vector[0];
                dronePosition_.y = vector[1];
                dronePosition_.z = vector[2];
                //printf("Drone position [%f,%f,%f] \n", vector[0], vector[1], vector[2]);
            }

            if (simxGetObjectVelocity(clientID_, droneHandler_, vector, angle, simx_opmode_buffer) == simx_return_ok) {
                dataReceiver_.notify("velocity", CartesianVector(vector[0], vector[1], vector[2]));
            }

            if (simxGetObjectOrientation(clientID_, droneHandler_, -1, angle, simx_opmode_buffer) == simx_return_ok) {
                dataReceiver_.notify("rotation", EulerianVector(angle[0], angle[1], angle[2]));
                droneRotation_.phi = normAngle(degreesToRadians(angle[0]));
                droneRotation_.theta = normAngle(degreesToRadians(angle[1]));
                droneRotation_.psi = normAngle(degreesToRadians(angle[2]));
                //printf("XciVrep: drone rotation[%f,%f,%f] \n", droneRotation_.phi, droneRotation_.theta, droneRotation_.psi);
            }
        }
        else{
            XCS_LOG_WARN("Lost connection with v-rep simulator.");
        }

        //extApi_sleepMs(25);
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

void XciVrep::updateImages() {
    while (!endAll_) {
        if (simxGetConnectionId(clientID_) != -1) { // we have connection with simulation server
            simxUChar** image = nullptr;
            int resolution[] = {0, 0};
            
            if (simxGetVisionSensorImage(clientID_, frontCameraHandler_, resolution, image, 0, simx_opmode_buffer) == simx_error_noerror) {
                printf("Image resolution [%i,%i]", resolution[0], resolution[1]);
            }
            

            if (simxGetVisionSensorImage(clientID_, bottomCameraHandler_, resolution, image, 0, simx_opmode_buffer) == simx_error_noerror) {
                printf("Image resolution [%i,%i]", resolution[0], resolution[1]);
            }
        }
        
        //extApi_sleepMs(40);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}
// ================ public functions ================

XciVrep::XciVrep(DataReceiver& dataReceiver,
        std::string droneName,
        std::string frontCameraName,
        std::string bottomCameraName,
        std::string targetName,
        std::string address,
        int portNumber) :
  Xci(dataReceiver),
  inited_(false),
  droneName_(droneName),
  frontCameraName_(frontCameraName),
  bottomCameraName_(bottomCameraName),
  targetName_(targetName),
  address_(address),
  portNumber_(portNumber) {
    clientID_ = -1;
    endAll_ = false;
    configuration_["XCI_PARAM_FP_PERSISTENCE"] = "20";
}

XciVrep::~XciVrep() {
    endAll_ = true;

    if (updateThread_.joinable()){
        updateThread_.join();
    }

    if (videoThread_.joinable()){
        videoThread_.join();
    }

    simxStopSimulation(clientID_, simx_opmode_oneshot_wait);

    if (clientID_ != -1) {
        simxFinish(clientID_);
    }
}

std::string XciVrep::name() {
    return "xci_vrep";
}

SensorList XciVrep::sensorList() {
    SensorList sensorList;

    sensorList.push_back(Sensor("position", "POSITION_ABS", "xcs::CartesianVector"));
    sensorList.push_back(Sensor("velocity", "VELOCITY_ABS", "xcs::CartesianVector"));
    sensorList.push_back(Sensor("rotation", "ROTATION", "xcs::EulerianVector"));

    sensorList.push_back(Sensor("video_front", "CAMERA", "xcs::BitmapType"));
    sensorList.push_back(Sensor("video_bottom", "CAMERA", "xcs::BitmapType"));

    return sensorList;
}

std::string XciVrep::configuration(const std::string &key) {
    if (configuration_.count(key) > 0) {
        return configuration_[key];
    } else {
        return "";
    }
}

InformationMap XciVrep::configuration() {
    return configuration_;
}

SpecialCMDList XciVrep::specialCMD() {
    return SpecialCMDList();
}

void XciVrep::configuration(const std::string &key, const std::string &value) {
}

void XciVrep::configuration(const InformationMap &configuration) {
}

void XciVrep::command(const std::string &command) {
}

void XciVrep::flyControl(float roll, float pitch, float yaw, float gaz) {
    if (simxGetConnectionId(clientID_) != -1) { // we have connection with simulation server 
        float position[3];
        position[0] = dronePosition_.x + cos(droneRotation_.psi) * roll * POS_MULTI - sin(droneRotation_.psi) * pitch*POS_MULTI;
        position[1] = dronePosition_.y - sin(droneRotation_.psi) * roll * POS_MULTI - cos(droneRotation_.psi) * pitch*POS_MULTI;
        position[2] = dronePosition_.z + gaz*POS_MULTI;
        simxSetObjectPosition(clientID_, targetHandler_, -1, position, simx_opmode_oneshot);

        float angle[3];
        angle[0] = 0;
        angle[1] = 0;
        angle[2] = radiansToDegrees(normAngle(droneRotation_.psi + yaw));
        simxSetObjectOrientation(clientID_, targetHandler_, -1, angle, simx_opmode_oneshot);
    }
    else{
        XCS_LOG_WARN("New fly control without connected client.");
    }
}

void XciVrep::init() {
    dataReceiver_.enabled(true);
    if (inited_) {
        return;
    }
    inited_ = true;
    // connect to the remote simulator with reconnection
    clientID_ = simxStart((simxChar*)address_.c_str(), portNumber_, true, false, 2000, 5);

    if (clientID_ != -1) {
        int error = 0;
        error = simxStartSimulation(clientID_, simx_opmode_oneshot_wait);
        if (error != 0){
            XCS_LOG_ERROR("Cannot start v-rep simulation!");
            return;
        }

        int counter = 0;
        do{
            error = simxGetObjectHandle(clientID_, droneName_.c_str(), &droneHandler_, simx_opmode_oneshot_wait);
        } while (error != 0 && counter < ATTEMPT_COUNT);

        if (error == 0){
            simxGetObjectPosition(clientID_, droneHandler_, -1, NULL, simx_opmode_streaming);
            simxGetObjectVelocity(clientID_, droneHandler_, NULL, NULL, simx_opmode_streaming);
            simxGetObjectOrientation(clientID_, droneHandler_, -1, NULL, simx_opmode_streaming);
            updateThread_ = std::thread(&XciVrep::updateSensors, this);
        }
        else{
            XCS_LOG_ERROR("Cannot get handle on drone. With error: " << error);
        }

        counter = 0;
        do{
            error = simxGetObjectHandle(clientID_, targetName_.c_str(), &targetHandler_, simx_opmode_oneshot_wait);
        } while (error != 0 && counter < ATTEMPT_COUNT);
        if (error == 0){
            
        }
        else{
            XCS_LOG_ERROR("Cannot get handle on target. With error: " << error);
        }

        bool camera = false;
        counter = 0;
        do{
            error = simxGetObjectHandle(clientID_, frontCameraName_.c_str(), &frontCameraHandler_, simx_opmode_oneshot_wait);
        } while (error != 0 && counter < ATTEMPT_COUNT);
        if (error == 0){
            simxGetVisionSensorImage(clientID_, frontCameraHandler_, NULL, NULL, 0, simx_opmode_streaming_split + 4000);
            camera = true;
        }
        else{
            XCS_LOG_WARN("Cannot get handle on front Camera. With error: " << error);
        }

        counter = 0;
        do{
            error = simxGetObjectHandle(clientID_, bottomCameraName_.c_str(), &bottomCameraHandler_, simx_opmode_oneshot_wait);
        } while (error != 0 && counter < ATTEMPT_COUNT);
        if (error == 0){
            simxGetVisionSensorImage(clientID_, bottomCameraHandler_, NULL, NULL, 0, simx_opmode_streaming_split + 4000);
            camera = true;
        }else{
            XCS_LOG_WARN("Cannot get handle on bottom Camera. With error: " << error);
        }
        
        if (camera){
            videoThread_ = std::thread(&XciVrep::updateImages, this);
        }

        simxAddStatusbarMessage(clientID_, "Xci_v-rep have been connected.", simx_opmode_oneshot);
    } else {
        XCS_LOG_ERROR("Cannot connect with v-rep simulator.");
    }
}

void XciVrep::stop() {
    dataReceiver_.enabled(false);
}

extern "C" {

Xci* CreateXci(DataReceiver &dataReceiver) {
    return new XciVrep(dataReceiver);
}
}
