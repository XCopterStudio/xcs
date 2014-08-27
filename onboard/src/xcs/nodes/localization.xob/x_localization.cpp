#include <xcs/settings.hpp>
#include <xcs/logging.hpp>
#include <xcs/xcs_fce.hpp>

#include "x_localization.hpp"

#include <urbi/uconversion.hh>
#include <sstream>
#include <limits>

using namespace std;
using namespace xcs;
using namespace xcs::nodes;
using namespace xcs::nodes::localization;

const double XLocalization::IMU_DELAY = 0.030; // 40ms
const double XLocalization::FLY_CONTROL_SEND_TIME = 0.030; // 75ms
const double XLocalization::CAM_DELAY = 0.100; // 

const std::string XLocalization::CTRL_INIT_KF = "init";
const std::string XLocalization::CTRL_TAKE_KF = "keyframe";
const std::string XLocalization::CTRL_RESET_PTAM = "resetPtam";
const std::string XLocalization::CTRL_RESET_EKF = "resetEkf";

void XLocalization::onChangeVelocity(xcs::CartesianVector measuredVelocity) {
    lastMeasurement_.velocity.x = measuredVelocity.x;
    lastMeasurement_.velocity.y = measuredVelocity.y;
}

void XLocalization::onChangeRotation(xcs::EulerianVector measuredRotation) {
    lastMeasurement_.velocityPsi = measuredRotation.psi - lastMeasurement_.rotation.psi;
    lastMeasurement_.rotation = measuredRotation;
}

void XLocalization::onChangeAltitude(double altitude) {
    lastMeasurement_.velocity.z = (altitude - lastMeasurement_.altitude);
    lastMeasurement_.altitude = altitude;
}

void XLocalization::onChangeTimeImu(double internalTime) {
    if (imuTimeShift_ == std::numeric_limits<double>::max()) {
        imuTimeShift_ = internalTime + IMU_DELAY - timeFromStart();
    }

    double ekfTime = internalTime - imuTimeShift_;
    if (std::abs(ekfTime - lastMeasurementTime_) > 1e-10) {
        if (std::abs(lastMeasurement_.velocityPsi) > M_PI_4) { // discard bigger change than M_PI_4
            lastMeasurement_.velocityPsi = 0;
        }

        lastMeasurement_.velocity.z /= std::abs(ekfTime - lastMeasurementTime_);
        lastMeasurement_.velocityPsi /= std::abs(ekfTime - lastMeasurementTime_);
   
        //printf("%f psiVelocity %f\n", ekfTime, lastMeasurement_.velocityPsi);

        lastMeasurementTime_ = ekfTime;
        // TODO: compute measurement time delay
        ekf_.measurementImu(lastMeasurement_, ekfTime);
        ptam_->measurementImu(lastMeasurement_, ekfTime);

        //printf("Measurement time %f actual time %f \n", actualTime, timeFromStart());

        // update future position of drone
        DroneState state = ekf_.computeState(timeFromStart() + flyControlSendTime_);
        // update actual position of drone
        //DroneState state = ekf_.computeState(timeFromStart());
        position = state.position;
        velocity = state.velocity;
        rotation = state.rotation;
        velocityPsi = state.velocityPsi;
    }
}

void XLocalization::onChangeVideo(urbi::UImage image) {
    if (!ptamEnabled_) {
        return;
    }
    lock_guard<mutex> lock(lastFrameMtx_);
    // store image until onChangeVideoTime
    lastFrame_ = image;
}

void XLocalization::onChangeVideoTime(xcs::Timestamp internalTime) {
    if (lastFrame_.data == nullptr){
        return;
    }

    if (!ptamEnabled_) {
        ptamStatus = static_cast<int> (PTAM_DISABLED);
        return;
    }
    double ekfTime = internalTime - imuTimeShift_;

    // convert image to grayscale for PTAM
    urbi::UImage bwImage;
    bwImage.imageFormat = urbi::IMAGE_GREY8;
    bwImage.data = nullptr;
    bwImage.size = 0;
    bwImage.width = 0;
    bwImage.height = 0;
    {
        lock_guard<mutex> lock(lastFrameMtx_);
        urbi::convert(lastFrame_, bwImage);
    }
    ptam_->handleFrame(bwImage, ekfTime); // this will do update EKF

    // update PTAM status
    ptamStatus = static_cast<int> (ptam_->ptamStatus());

    // update current state of drone
    DroneState state = ekf_.computeState(timeFromStart() + flyControlSendTime_);
    position = state.position;
    velocity = state.velocity;
    rotation = state.rotation;
}

void XLocalization::onChangeFlyControl(const xcs::FlyControl flyControl) {
    //printf("FlyControl time %f\n", timeFromStart());
    xcs::FlyControl boundedControl;
    boundedControl.roll = xcs::valueInRange(flyControl.roll, 0.0, 0.5);
    boundedControl.pitch = xcs::valueInRange(flyControl.pitch, 0.0, 0.5);
    boundedControl.yaw = xcs::valueInRange(flyControl.yaw, 0.0, 1.0);
    boundedControl.gaz = xcs::valueInRange(flyControl.gaz, 0.0, 1.0);
    ekf_.flyControl(boundedControl, timeFromStart() + FLY_CONTROL_SEND_TIME);
}

void XLocalization::onChangeFlyControlSendTime(const double flyControlSendTime){
    if (flyControlSendTime >= 0){
        flyControlSendTime_ = flyControlSendTime;
    }
}

void XLocalization::onChangeControl(const std::string &control) {
    if (control == CTRL_INIT_KF) {
        ptam_->takeInitKF();
    } else if (control == CTRL_TAKE_KF) {
        ptam_->takeKF();
    } else if (control == CTRL_RESET_PTAM) {
        ptam_->reset();
    } else if (control == CTRL_RESET_EKF) {
        ekf_.reset();
    } else {
        XCS_LOG_WARN("Unknown PTAM control '" << control << "'." << endl);
    }

}

void XLocalization::onChangePtamEnabled(const bool ptamEnabled) {
    ptamEnabled_ = ptamEnabled;
}

void XLocalization::onChangeSetPosition(xcs::CartesianVector position){
    ekf_.setPosition(position, timeFromStart());
}

void XLocalization::onChangeSetRotation(xcs::EulerianVector rotation){
    ekf_.setRotation(rotation, timeFromStart());
}

//================ public functions ==================

XLocalization::XLocalization(const std::string &name) :
  XObject(name),
  measuredVelocity("VELOCITY_LOC"),
  measuredRotation("ROTATION"),
  measuredAltitude("ALTITUDE"),
  timeImu("TIME_LOC"),
  video("CAMERA"),
  videoTime("TIME_LOC"),
  flyControl("FLY_CONTROL"),
  flyControlSendTime("TIME"),
  control("CONTROL"),
  ptamEnabled("ENABLED"),
  setPosition("POSITION_ABS"),
  setRotation("ROTATION"),
  position("POSITION_ABS"),
  velocity("VELOCITY_ABS"),
  rotation("ROTATION"),
  velocityPsi("ROTATION_VELOCITY_ABS"),
  ptamStatus("PTAM_STATUS") {
    lastFrame_.data = nullptr;
    startTime_ = clock_.now();
    lastMeasurementTime_ = 0;
    flyControlSendTime_ = FLY_CONTROL_SEND_TIME;
    imuTimeShift_ = std::numeric_limits<double>::max();
    ptamEnabled_ = true;

    XBindVarF(measuredVelocity, &XLocalization::onChangeVelocity);
    XBindVarF(measuredRotation, &XLocalization::onChangeRotation);
    XBindVarF(measuredAltitude, &XLocalization::onChangeAltitude);
    XBindVarF(timeImu, &XLocalization::onChangeTimeImu);

    XBindVarF(video, &XLocalization::onChangeVideo);
    XBindVar(videoTime);
    UNotifyThreadedChange(videoTime.data(), &XLocalization::onChangeVideoTime, urbi::LOCK_FUNCTION_DROP);

    XBindVarF(flyControl, &XLocalization::onChangeFlyControl);
    XBindVarF(flyControlSendTime, &XLocalization::onChangeFlyControlSendTime);
    XBindVarF(control, &XLocalization::onChangeControl);
    XBindVarF(ptamEnabled, &XLocalization::onChangePtamEnabled);

    XBindVarF(setPosition, &XLocalization::onChangeSetPosition);
    XBindVarF(setRotation, &XLocalization::onChangeSetRotation);

    XBindVar(position);
    XBindVar(velocity);
    XBindVar(rotation);
    XBindVar(velocityPsi);
    XBindVar(ptamStatus);

    XBindFunction(XLocalization, init);
}

void XLocalization::init(const std::string &configFile) {
    ptam_ = PtamPtr(new Ptam(ekf_));

    loadParameters(Settings(configFile));
}

void XLocalization::loadParameters(const Settings& settings) {
    try {
        /*
         * EKF parameters
         */
        Parameters modelParameters(10, 0);
        for (auto i = 0; i < 10; ++i) {
            stringstream name;
            name << "Ekf.ModelParameters.c" << i;
            modelParameters[i] = settings.get<double>(name.str());
        }
        ekf_.modelParameters(modelParameters);

        Variances modelVar(4, 0);
        modelVar[0] = settings.get<double>("Ekf.ModelVariance.roll");
        modelVar[1] = settings.get<double>("Ekf.ModelVariance.pitch");
        modelVar[2] = settings.get<double>("Ekf.ModelVariance.yaw");
        modelVar[3] = settings.get<double>("Ekf.ModelVariance.gaz");
        ekf_.modelVariances(modelVar);

        Variances imuVar(6, 0);
        imuVar[0] = settings.get<double>("Ekf.ImuVariance.velocityX");
        imuVar[1] = settings.get<double>("Ekf.ImuVariance.velocityY");
        imuVar[2] = settings.get<double>("Ekf.ImuVariance.velocityZ");
        imuVar[3] = settings.get<double>("Ekf.ImuVariance.phi");
        imuVar[4] = settings.get<double>("Ekf.ImuVariance.theta");
        imuVar[5] = settings.get<double>("Ekf.ImuVariance.velocityPsi");
        ekf_.imuVariances(imuVar);

        Variances ptamVar(6, 0);
        ptamVar[0] = settings.get<double>("Ekf.PtamVariance.x");
        ptamVar[1] = settings.get<double>("Ekf.PtamVariance.y");
        ptamVar[2] = settings.get<double>("Ekf.PtamVariance.z");
        ptamVar[3] = settings.get<double>("Ekf.PtamVariance.phi");
        ptamVar[4] = settings.get<double>("Ekf.PtamVariance.theta");
        ptamVar[5] = settings.get<double>("Ekf.PtamVariance.psi");
        ekf_.ptamVariances(ptamVar);

        /*
         * PTAM parameters
         */
        Ptam::CameraParameters cameraParameters;
        for (auto i = 0; i < 5; ++i) {
            stringstream name;
            name << "Ptam.CameraParameters.c" << i;
            cameraParameters.push_back(settings.get<double>(name.str()));
        }
        ptam_->cameraParameters(cameraParameters);

        ptam_->parameters(settings.getMap<double>("Ptam.Ptam"));

    } catch (boost::property_tree::ptree_error error) {
        XCS_LOG_ERROR("Localization: Cannot load parameters from file. Error: " << error.what());
    }
}

XStart(XLocalization);

