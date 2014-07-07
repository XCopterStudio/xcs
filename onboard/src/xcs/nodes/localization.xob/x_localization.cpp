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

const double XLocalization::IMU_DELAY = 0.040; // 40ms
const double XLocalization::FLY_CONTROL_SEND_TIME = 0.075; // 75ms
const double XLocalization::CAM_DELAY = 0.100; // 

void XLocalization::onChangeVelocity(xcs::CartesianVector measuredVelocity) {
    lastMeasurement_.velocity.x = measuredVelocity.x;
    lastMeasurement_.velocity.y = measuredVelocity.y;
}

void XLocalization::onChangeRotation(xcs::EulerianVector measuredAnglesRotation) {
    lastMeasurement_.angularRotationPsi = measuredAnglesRotation.psi - lastMeasurement_.angles.psi;
    lastMeasurement_.angles = measuredAnglesRotation;
}

void XLocalization::onChangeAltitude(double altitude) {
    lastMeasurement_.velocity.z = (altitude - lastMeasurement_.altitude);
    lastMeasurement_.altitude = altitude;
}

void XLocalization::onChangeTimeImu(double internalTime) {
    if (imuTimeShift_ == std::numeric_limits<double>::max()) {
        imuTimeShift_ = internalTime - timeFromStart();
    }

    double ekfTime = internalTime - imuTimeShift_ - IMU_DELAY;
    if (std::abs(ekfTime - lastMeasurementTime_) > 1e-10){

        lastMeasurement_.velocity.z /= std::abs(ekfTime - lastMeasurementTime_);
        lastMeasurement_.angularRotationPsi /= std::abs(ekfTime - lastMeasurementTime_);

        if (std::abs(lastMeasurement_.angularRotationPsi) > M_PI_4) { // discard bigger change than M_PI_4
            lastMeasurement_.angularRotationPsi = 0;
        }

        lastMeasurementTime_ = ekfTime;
        // TODO: compute measurement time delay
        ekf_.measurementImu(lastMeasurement_, ekfTime);
        ptam_->measurementImu(lastMeasurement_, ekfTime);

        //printf("Measurement time %f actual time %f \n", actualTime, timeFromStart());

        // update actual position of drone
        DroneState state = ekf_.computeState(timeFromStart());
        position = state.position;
        velocity = state.velocity;
        rotation = state.angles;
        velocityPsi = state.angularRotationPsi;
    }
}

void XLocalization::onChangeVideo(urbi::UImage image) {
    lock_guard<mutex> lock(lastFrameMtx_);
    // store image until onChangeVideoTime
    lastFrame_ = image;
    lastFrameTime_ = clock_.now();
}

void XLocalization::onChangeVideoTime(xcs::Timestamp internalTime) {

    if (duration2sec(clock_.now() - lastFrameTime_) > 0.05) {
        return;
    }
    double ekfTime = internalTime - imuTimeShift_ - CAM_DELAY;

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

    // update current state of drone
    DroneState state = ekf_.computeState(timeFromStart());
    position = state.position;
    velocity = state.velocity;
    rotation = state.angles;
}

void XLocalization::onChangeFlyControl(const xcs::FlyControl flyControl) {
    //printf("FlyControl time %f\n", timeFromStart());
    ekf_.flyControl(flyControl, timeFromStart() + FLY_CONTROL_SEND_TIME); // TODO: compute flyControl delay
}

void XLocalization::onChangePtamControl(const std::string &ptamControl) {
    std::cerr << "PTAM control " << ptamControl << std::endl;
    ptam_->pressSpacebar();
}

//================ public functions ==================

XLocalization::XLocalization(const std::string &name) :
  XObject(name),
  measuredVelocity("VELOCITY"),
  measuredAnglesRotation("ROTATION"),
  measuredAltitude("ALTITUDE"),
  timeImu("TIME_LOC"),
  video("FRONT_CAMERA"),
  videoTime("TIME_LOC"),
  flyControl("FLY_CONTROL"),
  ptamControl("COMMAND"),
  position("POSITION_ABS"),
  velocity("VELOCITY_ABS"),
  rotation("ROTATION"),
  velocityPsi("ROTATION_VELOCITY_ABS") {
    startTime_ = clock_.now();
    lastMeasurementTime_ = 0;
    imuTimeShift_ = std::numeric_limits<double>::max();

    XBindVarF(measuredVelocity, &XLocalization::onChangeVelocity);
    XBindVarF(measuredAnglesRotation, &XLocalization::onChangeRotation);
    XBindVarF(measuredAltitude, &XLocalization::onChangeAltitude);
    XBindVarF(timeImu, &XLocalization::onChangeTimeImu);

    XBindVarF(video, &XLocalization::onChangeVideo);
    XBindVar(videoTime);
    UNotifyThreadedChange(videoTime.Data(), &XLocalization::onChangeVideoTime, urbi::LOCK_FUNCTION);

    XBindVarF(flyControl, &XLocalization::onChangeFlyControl);
    XBindVarF(ptamControl, &XLocalization::onChangePtamControl);

    XBindVar(position);
    XBindVar(velocity);
    XBindVar(rotation);
    XBindVar(velocityPsi);

    XBindFunction(XLocalization, init);
    XBindFunction(XLocalization, loadParameters);
}

void XLocalization::init() {
    ptam_ = PtamPtr(new Ptam(ekf_));
}

void XLocalization::loadParameters(const std::string &file) {
    Settings settings(file);

    try {
        Parameters modelParameters(10, 0);
        for (unsigned int i = 0; i < 10; ++i) {
            stringstream name;
            name << "ModelParameters.c" << i;
            modelParameters[i] = settings.get<double>(name.str());
        }
        ekf_.modelParameters(modelParameters);

        Variances modelVar(4, 0);
        modelVar[0] = settings.get<double>("ModelVariance.roll");
        modelVar[1] = settings.get<double>("ModelVariance.pitch");
        modelVar[2] = settings.get<double>("ModelVariance.yaw");
        modelVar[3] = settings.get<double>("ModelVariance.gaz");
        ekf_.modelVariances(modelVar);

        Variances imuVar(7, 0);
        imuVar[0] = settings.get<double>("ImuVariance.z");
        imuVar[1] = settings.get<double>("ImuVariance.velocityX");
        imuVar[2] = settings.get<double>("ImuVariance.velocityY");
        imuVar[3] = settings.get<double>("ImuVariance.velocityZ");
        imuVar[4] = settings.get<double>("ImuVariance.phi");
        imuVar[5] = settings.get<double>("ImuVariance.theta");
        imuVar[6] = settings.get<double>("ImuVariance.velocityPsi");
        ekf_.imuVariances(imuVar);

        Variances ptamVar(6, 0);
        ptamVar[0] = settings.get<double>("PtamVariance.x");
        ptamVar[1] = settings.get<double>("PtamVariance.y");
        ptamVar[2] = settings.get<double>("PtamVariance.z");
        ptamVar[3] = settings.get<double>("PtamVariance.phi");
        ptamVar[4] = settings.get<double>("PtamVariance.theta");
        ptamVar[5] = settings.get<double>("PtamVariance.psi");
        ekf_.ptamVariances(ptamVar);
    } catch (boost::property_tree::ptree_error error) {
        XCS_LOG_ERROR("Localization cannot load parameters from file " << file << "\n With boost error: " << error.what());
    }
}

XStart(XLocalization);

