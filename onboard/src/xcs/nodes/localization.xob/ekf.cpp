#include "ekf.hpp"

#include <xcs/logging.hpp>
#include <xcs/xcs_fce.hpp>

#include <cmath>

#define ARMA_NO_DEBUG

using namespace arma;
using namespace boost;
using namespace std;
using namespace xcs;
using namespace xcs::nodes::localization;

template <typename Deque>
void Ekf::clearUpToTime(Deque &deque, const double &time) {
    int index = findNearest(deque, time);
    for (int i = 0; i < index; ++i) {
        deque.pop_front();
    }
}

template <typename Deque>
int Ekf::findNearest(Deque &deque, const double &time) {
    if (deque.front().second > time) { // We do not have any elements in deque which time is lesser than input time
        return -1;
    }

    int first = 0;
    int last = deque.size() - 1;

    if (deque[last].second <= time) {
        return last;
    }

    while (first <= last) {
        int middle = (first + last) / 2;
        if (deque[middle].second <= time && deque[middle + 1].second > time) {
            return middle;
        } else if (deque[middle].second < time) {
            first = middle + 1;
        } else {
            last = middle - 1;
        }
    }

    return -1;
}

int Ekf::findMeasurementIndex(const int &ID) {
    int first = 0;
    int last = imuMeasurements_.size() - 1;

    while (first <= last) {
        int middle = (first + last) / 2;
        if (imuMeasurements_[middle].first.measurementID == ID) {
            return middle;
        } else if (imuMeasurements_[middle].first.measurementID < ID) {
            first = middle + 1;
        } else {
            last = middle - 1;
        }
    }

    return -1;
}

DroneStateDistributionChronologic Ekf::predict(const DroneStateDistributionChronologic& state, const double &endTime) {
    DroneStateDistributionChronologic newState = state;
    double flyControlTime = state.second;
    XCS_LOG_INFO("Predict up to time %f" << flyControlTime);
    int controlIndex = findNearest(flyControls_, flyControlTime);

    do { // predict state up to the measurement time
        double nextControlTime = 0;
        if (controlIndex < (flyControls_.size() - 1) && controlIndex != -1) {
            nextControlTime = flyControls_[controlIndex + 1].second;
        } else {
            nextControlTime = std::numeric_limits<double>::max();
        }
 
        double delta = std::min(nextControlTime, endTime) - flyControlTime;
        if (controlIndex >= 0){
            newState.first = predict(newState.first, flyControls_[controlIndex].first, delta);
        }
        else{
            newState.first = predict(newState.first, xcs::FlyControl(), delta);
        }


        flyControlTime = nextControlTime;
        ++controlIndex;
    } while (flyControlTime < endTime);

    newState.second = endTime;
    return newState;
}

DroneStateDistributionChronologic Ekf::predictAndUpdateFromImu(const DroneStateDistributionChronologic& state, const double &endTime, bool saveInterResults) {
    DroneStateDistributionChronologic newState = state;

    int controlsIndex = findNearest(flyControls_, newState.second);
    int measurementIndex = findMeasurementIndex(state.first.first.updateMeasurementID) + 1;

    do {
        if (measurementIndex < imuMeasurements_.size()) {
            double measurementTime = imuMeasurements_[measurementIndex].second;
            newState = predict(newState, measurementTime);
        } else {
            newState = predict(newState, endTime);
            break;
        }

        //M: printf("EKF: Drone measurement %i [%f,%f,%f,%f,%f,%f,%f]\n",
        //            measurementIndex,
        //            imuMeasurements_[measurementIndex].first.altitude,
        //            imuMeasurements_[measurementIndex].first.velocity.x,
        //            imuMeasurements_[measurementIndex].first.velocity.y,
        //            imuMeasurements_[measurementIndex].first.velocity.z,
        //            imuMeasurements_[measurementIndex].first.angles.phi,
        //            imuMeasurements_[measurementIndex].first.angles.theta,
        //            imuMeasurements_[measurementIndex].first.angularRotationPsi);

        newState.first = updateIMU(newState.first, imuMeasurements_[measurementIndex].first);
        measurementIndex++;
        if (saveInterResults) {
            droneStates_.push_back(newState);
        }
    } while (newState.second < endTime);

    return newState;
}

DroneStateDistribution Ekf::predict(const DroneStateDistribution &state, const FlyControl &flyControl, const double &delta) {
    DroneStateDistribution newState = state;
    const CartesianVector &positionOld = state.first.position;
    const CartesianVector &velocityOld = state.first.velocity;
    const EulerianVector &anglesOld = state.first.angles;

    //M: printf("EKF: flyControl [%f,%f,%f,%f] \n", flyControl.roll, flyControl.pitch, flyControl.yaw, flyControl.gaz);

    // predict acceleration
    // acceleration in drone frame
    double force = modelPar_[0] * (1.0 + modelPar_[1] * flyControl.gaz);
    double forceX = force * sin(anglesOld.phi);
    double forceY = -force * cos(anglesOld.phi) * sin(anglesOld.theta);
    ////M: printf("EKF: Force [%f,%f] \n",forceX,forceY);
    // drag
    double dragX = modelPar_[2] * velocityOld.x + modelPar_[3] * velocityOld.x * velocityOld.x;
    double dragY = modelPar_[2] * velocityOld.y + modelPar_[3] * velocityOld.y * velocityOld.y;
    ////M: printf("EKF: Drag[%f,%f] \n", dragX,dragY);
    // drone acceleration in global frame
    CartesianVector acceleration;
    acceleration.x = (cos(anglesOld.psi) * forceX + sin(anglesOld.psi) * forceY) - dragX;
    acceleration.y = (-sin(anglesOld.psi) * forceX + cos(anglesOld.psi) * forceY) - dragY;
    acceleration.z = modelPar_[8] * flyControl.gaz - modelPar_[9] * velocityOld.z;
    ////M: printf("EKF: Acceleration [%f,%f,%f] \n", acceleration.x, acceleration.y, acceleration.z);

    // angular rotation speed
    EulerianVector angularRotation;
    angularRotation.phi = modelPar_[4] * flyControl.roll - modelPar_[5] * anglesOld.phi;
    angularRotation.theta = modelPar_[4] * flyControl.pitch - modelPar_[5] * anglesOld.theta;
    // angular acceleration
    angularRotation.psi = modelPar_[6] * flyControl.yaw - modelPar_[7] * state.first.velocityPsi;
    ////M: printf("EKF: Rotation [%f,%f,%f] \n", angularRotation.phi, angularRotation.theta, angularRotation.psi);

    // =========== predict new state ============
    // position
    CartesianVector &position = newState.first.position;
    position += velocityOld * delta;
    // velocity
    CartesianVector &velocity = newState.first.velocity;
    velocity += acceleration * delta;
    ////M: printf("EKF: velocity[%f,%f,%f]\n", velocity.x, velocity.y, velocity.z);
    // angles
    EulerianVector &angles = newState.first.angles;
    angles.phi = xcs::normAngle(angles.phi + angularRotation.phi * delta);
    angles.theta = xcs::normAngle(angles.theta + angularRotation.theta * delta);
    angles.psi = xcs::normAngle(angles.psi + state.first.velocityPsi * delta);
    // angular rotation psi
    newState.first.velocityPsi += angularRotation.psi*delta;
    // =========== end predict new state ========

    // create jacobian matrix
    mat jacobian(10, 10, fill::zeros);
    // position x
    jacobian(0, 0) = 1;
    jacobian(0, 3) = delta;
    // position y
    jacobian(1, 1) = 1;
    jacobian(1, 4) = delta;
    // position z
    jacobian(2, 2) = 1;
    jacobian(2, 5) = delta;
    // velocity x
    jacobian(3, 3) = 1 - modelPar_[2] * delta - modelPar_[3] * 2 * delta * velocityOld.x;
    jacobian(3, 6) = delta * force * (
            cos(anglesOld.phi) * cos(anglesOld.psi)
            + sin(anglesOld.phi) * sin(anglesOld.theta) * sin(anglesOld.psi)
            );
    jacobian(3, 7) = delta * force * (
            -cos(anglesOld.phi) * cos(anglesOld.theta) * sin(anglesOld.psi)
            );
    jacobian(3, 8) = delta * force * (
            -sin(anglesOld.phi) * sin(anglesOld.psi)
            - cos(anglesOld.phi) * sin(anglesOld.theta) * cos(anglesOld.psi)
            );
    // velocity y
    jacobian(4, 4) = 1 - modelPar_[2] * delta - modelPar_[3] * 2 * delta * velocityOld.y;
    jacobian(4, 6) = delta * force * (
            -cos(anglesOld.phi) * sin(anglesOld.psi)
            + sin(anglesOld.phi) * sin(anglesOld.theta) * cos(anglesOld.psi)
            );
    jacobian(4, 7) = delta * force * (
            -cos(anglesOld.phi) * cos(anglesOld.theta) * cos(anglesOld.psi)
            );
    jacobian(4, 8) = delta * force * (
            -sin(anglesOld.phi) * cos(anglesOld.psi)
            + cos(anglesOld.phi) * sin(anglesOld.theta) * sin(anglesOld.psi)
            );
    // velocity z
    jacobian(5, 5) = 1 - delta * modelPar_[9];
    // angle phi
    jacobian(6, 6) = 1 - delta * modelPar_[5];
    // angle theta
    jacobian(7, 7) = 1 - delta * modelPar_[5];
    // angle psi
    jacobian(8, 8) = 1;
    jacobian(8, 9) = delta;
    // rotation speed psi
    jacobian(9, 9) = 1 - delta * modelPar_[7];

    // normal noise
    mat noiseTransf(10, 4, fill::zeros);
    // phi
    noiseTransf(6, 0) = delta * modelPar_[4];
    // theta
    noiseTransf(7, 1) = delta * modelPar_[4];
    // rotation speed psi
    noiseTransf(9, 2) = delta * modelPar_[6];
    // gaz
    noiseTransf(3, 3) = delta * modelPar_[0] * modelPar_[1] * (
            sin(anglesOld.phi) * cos(anglesOld.psi)
            - cos(anglesOld.phi) * sin(anglesOld.theta)) * sin(anglesOld.psi);
    noiseTransf(4, 3) = delta * modelPar_[0] * modelPar_[1] * (
            -sin(anglesOld.phi) * sin(anglesOld.psi)
            - cos(anglesOld.phi) * sin(anglesOld.theta) * cos(anglesOld.psi));
    noiseTransf(5, 3) = delta * modelPar_[8];

    // ======= predict state deviation ===========
    mat noise(4, 4, fill::zeros); // TODO: compute better noise
    noise(0, 0) = modelVariance_[0];
    noise(1, 1) = modelVariance_[1];
    noise(2, 2) = modelVariance_[2];
    noise(3, 3) = modelVariance_[3];

    newState.second = jacobian * state.second * jacobian.t() + noiseTransf * noise * noiseTransf.t();

    // ======= end predict state deviation ===========

    ////M: printf("EKF: delta %f\n", delta);
    //M: printf("EKF: Computed drone predictedState [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
    //        newState.first.position.x, newState.first.position.y, newState.first.position.z,
    //        newState.first.velocity.x, newState.first.velocity.y, newState.first.velocity.z,
    //        newState.first.angles.phi, newState.first.angles.theta, newState.first.angles.psi,
    //        newState.first.velocityPsi);
    //////M: printf("EKF: Deviation drone predictedState (%f,%f,%f,%f,%f,%f,%f,%f,%f,%f)\n",
    //    newState.second(0, 0), newState.second(1, 1), newState.second(2, 2),
    //    newState.second(3, 3), newState.second(4, 4), newState.second(5, 5),
    //    newState.second(6, 6), newState.second(7, 7), newState.second(8, 8),
    //    newState.second(9, 9));

    return newState;
}

DroneStateDistribution Ekf::updateIMU(const DroneStateDistribution &state, const DroneStateMeasurement &imuMeasurement) {
    // create jacobian from measurement function
    mat measurementJacobian(7, 10, fill::zeros);
    // altitude
    measurementJacobian(0, 2) = 1;
    // acceleration x
    measurementJacobian(1, 3) = cos(state.first.angles.psi);
    measurementJacobian(1, 4) = sin(state.first.angles.psi);
    measurementJacobian(1, 8) = -state.first.velocity.x * sin(state.first.angles.psi)
            + state.first.velocity.y * cos(state.first.angles.psi);
    // acceleration y
    measurementJacobian(2, 3) = -sin(state.first.angles.psi);
    measurementJacobian(2, 4) = cos(state.first.angles.psi);
    measurementJacobian(2, 8) = -state.first.velocity.x * cos(state.first.angles.psi)
            - state.first.velocity.y * sin(state.first.angles.psi);
    // acceleration z
    measurementJacobian(3, 5) = 1;
    // phi
    measurementJacobian(4, 6) = 1;
    // theta
    measurementJacobian(5, 7) = 1;
    // psi
    measurementJacobian(6, 9) = 1;

    // additional noise // TODO: compute better values
    mat noise(7, 7, fill::zeros);
    noise(0, 0) = imuVariance_[0]; // altitude variance 
    noise(1, 1) = imuVariance_[1]; // velocity x
    noise(2, 2) = imuVariance_[2]; // velocity y
    noise(3, 3) = imuVariance_[3]; // velocity z
    noise(4, 4) = imuVariance_[4]; // phi 
    noise(5, 5) = imuVariance_[5]; // theta
    noise(6, 6) = imuVariance_[6]; // angular velocity psi

    // compute kalman gain
    mat gain = state.second * measurementJacobian.t() * (measurementJacobian * state.second * measurementJacobian.t() + noise).i();

    // compute predicted measurement 
    mat predictedMeasurement(7, 1);
    predictedMeasurement(0, 0) = state.first.position.z;
    predictedMeasurement(1, 0) = state.first.velocity.x * cos(state.first.angles.psi)
            + state.first.velocity.y * sin(state.first.angles.psi);
    predictedMeasurement(2, 0) = -state.first.velocity.x * sin(state.first.angles.psi)
            + state.first.velocity.y * cos(state.first.angles.psi);
    predictedMeasurement(3, 0) = state.first.velocity.z;
    predictedMeasurement(4, 0) = state.first.angles.phi;
    predictedMeasurement(5, 0) = state.first.angles.theta;
    predictedMeasurement(6, 0) = state.first.velocityPsi;

    // update state
    mat newStateMean = static_cast<mat> (state.first) + gain * (static_cast<mat> (imuMeasurement) - predictedMeasurement);

    /* printf("Predicted measurement \n");
     predictedMeasurement.print();
     printf("Imu measurement \n");
     static_cast<mat>(imuMeasurement).print();
     printf("New state");
     newStateMean.print();*/

    DroneStateDistribution newState;
    newState.first = newStateMean;
    newState.first.updateMeasurementID = imuMeasurement.measurementID;

    // update deviation
    newState.second = (mat(10, 10).eye() - gain*measurementJacobian) * state.second;

    /*mat error = (newStateMean - static_cast<mat>(state.first))*(newStateMean - static_cast<mat>(state.first)).t();
    printf("error %.10e %.10e %.10e %.10e %.10e %.10e %.10e %.10e %.10e %.10e \n", 
        error.at(0, 0),
        error.at(1, 1),
        error.at(2, 2), 
        error.at(3, 3), 
        error.at(4, 4), 
        error.at(5, 5), 
        error.at(6, 6),
        error.at(7, 7),
        error.at(8, 8),
        error.at(9, 9));*/

    //M: printf("EKF: Computed drone updatedState [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
    //        newState.first.position.x, newState.first.position.y, newState.first.position.z,
    //        newState.first.velocity.x, newState.first.velocity.y, newState.first.velocity.z,
    //        newState.first.angles.phi, newState.first.angles.theta, newState.first.angles.psi,
    //        newState.first.velocityPsi);

    return newState;
}

DroneStateDistribution Ekf::updateCam(const DroneStateDistribution &state, const CameraMeasurement &camMeasurement) {
    mat measurementJacobian(6, 10, fill::zeros);
    measurementJacobian(0, 0) = 1; // x
    measurementJacobian(1, 1) = 1; // y
    measurementJacobian(2, 2) = 1; // z
    measurementJacobian(3, 6) = 1; // phi
    measurementJacobian(4, 7) = 1; // theta
    measurementJacobian(5, 8) = 1; // psi

    mat noise(6, 6, fill::zeros);
    noise(0, 0) = ptamVariance_[0]; // x 
    noise(1, 1) = ptamVariance_[1]; // y
    noise(2, 2) = ptamVariance_[2]; // z
    noise(3, 3) = ptamVariance_[3]; // phi
    noise(4, 4) = ptamVariance_[4]; // theta 
    noise(5, 5) = ptamVariance_[5]; // psi

    // compute kalman gain
    mat gain = state.second * measurementJacobian.t() * (measurementJacobian * state.second * measurementJacobian.t() + noise).i();

    // compute predicted measurement 
    mat predictedMeasurement(6, 1);
    predictedMeasurement(0, 0) = state.first.position.x;
    predictedMeasurement(1, 0) = state.first.position.y;
    predictedMeasurement(2, 0) = state.first.position.z;
    predictedMeasurement(3, 0) = state.first.angles.phi;
    predictedMeasurement(4, 0) = state.first.angles.theta;
    predictedMeasurement(5, 0) = state.first.angles.psi;

    // update state
    DroneStateDistribution newState;
    mat newStateMean = static_cast<mat> (state.first) + gain * (static_cast<mat> (camMeasurement) - predictedMeasurement);
    newState.first = newStateMean;
    newState.first.updateMeasurementID = state.first.updateMeasurementID;

    // update deviation
    newState.second = (mat(10, 10).eye() - gain * measurementJacobian) * state.second;


    //M: printf("EKF: Computed drone updatedState [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
    //        newState.first.position.x, newState.first.position.y, newState.first.position.z,
    //        newState.first.velocity.x, newState.first.velocity.y, newState.first.velocity.z,
    //        newState.first.angles.phi, newState.first.angles.theta, newState.first.angles.psi,
    //        newState.first.velocityPsi);

    return newState;
}

// =========================== public functions ============================

Ekf::Ekf() {

    IDCounter_ = 1;

    // add default element in all queue
    flyControls_.push_back(FlyControlChronologic(FlyControl(), 0));
    imuMeasurements_.push_back(ImuMeasurementChronologic(DroneStateMeasurement(), 0));
    droneStates_.push_back(DroneStateDistributionChronologic(
            DroneStateDistribution(DroneState(), arma::mat(10, 10, fill::eye)),
            0));

    modelPar_[0] = 10.32;
    modelPar_[1] = 0; // TODO: compute
    modelPar_[2] = 0.58;
    modelPar_[3] = 0; // TODO: compute
    modelPar_[4] = 6.108;
    modelPar_[5] = 0.175;
    modelPar_[6] = 4.363;
    modelPar_[7] = 0.175;
    modelPar_[8] = 1.4;
    modelPar_[9] = 1.4;

    modelVariance_[0] = 3;
    modelVariance_[1] = 3;
    modelVariance_[2] = 9;
    modelVariance_[3] = 1;

    imuVariance_[0] = 0.0004;
    imuVariance_[1] = 0.000025;
    imuVariance_[2] = 0.000025;
    imuVariance_[3] = 0.01;
    imuVariance_[4] = 0.0003;
    imuVariance_[5] = 0.0003;
    imuVariance_[6] = 0.001;

    ptamVariance_[0] = 0.01;
    ptamVariance_[1] = 0.01;
    ptamVariance_[2] = 0.01;
    ptamVariance_[3] = 0.122;
    ptamVariance_[4] = 0.122;
    ptamVariance_[5] = 0.122;

}

Ekf::Ekf(Parameters parameters, Variances model, Variances imu, Variances ptam) {
    IDCounter_ = 1;

    // add default element in all queue
    flyControls_.push_back(FlyControlChronologic(FlyControl(), 0));
    imuMeasurements_.push_back(ImuMeasurementChronologic(DroneStateMeasurement(), 0));
    droneStates_.push_back(DroneStateDistributionChronologic(
            DroneStateDistribution(DroneState(), arma::mat(10, 10, fill::eye)),
            0));


    if (parameters.size() != 10 || model.size() != 4 || imu.size() != 7 || ptam.size() != 6) {
        XCS_LOG_ERROR("Incorrect parameters or variances array size. Should be parameters 10, model 4, imu 7, ptam 6.");
    } else {
        // set model parameters
        for (unsigned int i = 0; i < 10; ++i) {
            modelPar_[i] = parameters[i];
        }
        // set model variances
        for (unsigned int i = 0; i < 4; ++i) {
            modelVariance_[i] = model[i];
        }
        // set imu variances
        for (unsigned int i = 0; i < 7; ++i) {
            imuVariance_[i] = imu[i];
        }
        // set ptam variances
        for (unsigned int i = 0; i < 6; ++i) {
            ptamVariance_[i] = ptam[i];
        }
    }
}

void Ekf::modelParameters(Parameters parameters) {
    if (parameters.size() == 10) {
        for (unsigned int i = 0; i < 10; ++i) {
            modelPar_[i] = parameters[i];
        }
    } else {
        XCS_LOG_WARN("Incorrect parameters count. Should be 10.");
    }
}

void Ekf::modelVariances(Variances model) {
    if (model.size() == 4) {
        for (unsigned int i = 0; i < 4; ++i) {
            modelVariance_[i] = model[i];
        }
    } else {
        XCS_LOG_WARN("Incorrect model variances count. Should be 4.");
    }
}

void Ekf::imuVariances(Variances imu) {
    if (imu.size() == 7) {
        for (unsigned int i = 0; i < 7; ++i) {
            imuVariance_[i] = imu[i];
        }
    } else {
        XCS_LOG_WARN("Incorrect imu variances count. Should be 7.");
    }
}

void Ekf::ptamVariances(Variances ptam) {
    if (ptam.size() == 6) {
        for (unsigned int i = 0; i < 6; ++i) {
            ptamVariance_[i] = ptam[i];
        }
    } else {
        XCS_LOG_WARN("Incorrect ptam variances count. Should be 6.");
    }
}

void Ekf::clearUpToTime(const double timestamp) {
    unique_lock<shared_mutex> lock(bigSharedMtx_);
    clearUpToTime(droneStates_, timestamp);
    clearUpToTime(flyControls_, timestamp);
    clearUpToTime(imuMeasurements_, timestamp);
}

void Ekf::flyControl(const FlyControl flyControl, const double timestamp) {
    XCS_LOG_INFO("Inserted new flyControl with timestamp: " << timestamp);
    unique_lock<shared_mutex> lock(bigSharedMtx_);
    flyControls_.push_back(FlyControlChronologic(flyControl, timestamp));
};

void Ekf::measurementImu(const DroneStateMeasurement &measurement, const double timestamp) {
    unique_lock<shared_mutex> lock(bigSharedMtx_);
    ImuMeasurementChronologic copyMeasurement(measurement, timestamp);
    copyMeasurement.first.measurementID = IDCounter_++;
    XCS_LOG_INFO("Inserted new imu measurement with timestamp: " << timestamp);
    imuMeasurements_.push_back(copyMeasurement);
    int index = findNearest(droneStates_, timestamp); // TODO: check -1 when findNearest cannot find any state which we can update
    droneStates_.push_back(predictAndUpdateFromImu(droneStates_[index], timestamp));
    XCS_LOG_INFO("PTAM (imu): " << droneStates_.back().first.first.position.x << " " << droneStates_.back().first.first.position.y << " " << droneStates_.back().first.first.position.z << " " << droneStates_.back().first.first.angles.phi << " " << droneStates_.back().first.first.angles.theta << " " << droneStates_.back().first.first.angles.psi << " " << timestamp);
};

void Ekf::measurementCam(const CameraMeasurement &measurement, const double timestamp) {
    unique_lock<shared_mutex> lock(bigSharedMtx_); // Note: Maybe upgradable lock could be used, but this is not a conditinal writer.
    XCS_LOG_INFO("Inserted new camera measurement with timestamp: " << timestamp);
    int index = findNearest(droneStates_, timestamp);
    DroneStateDistributionChronologic newState = droneStates_[index];
    //delete all old droneStates
    droneStates_.clear();

    // create new droneStates up to the time of last imuMeasurements
    newState = predict(newState, timestamp);
    XCS_LOG_INFO("PTAM (cam_predict): " << newState.first.first.position.x << " " << newState.first.first.position.y << " " << newState.first.first.position.z << " " << newState.first.first.angles.phi << " " << newState.first.first.angles.theta << " " << newState.first.first.angles.psi << " " << timestamp);
    newState.first = updateCam(newState.first, measurement);
    XCS_LOG_INFO("PTAM (cam_update): " << newState.first.first.position.x << " " << newState.first.first.position.y << " " << newState.first.first.position.z << " " << newState.first.first.angles.phi << " " << newState.first.first.angles.theta << " " << newState.first.first.angles.psi << " " << timestamp);
    // save cam update
    droneStates_.push_back(newState);
    // update up to the last imu measurements
    predictAndUpdateFromImu(newState, imuMeasurements_.back().second, true);

    // clear old imuMeasurements and flyControls
    clearUpToTime(imuMeasurements_, timestamp);
    clearUpToTime(flyControls_, timestamp);
}

DroneState Ekf::computeState(const double time) {
    shared_lock<shared_mutex> lock(bigSharedMtx_);
    int index = findNearest(droneStates_, time);
    DroneStateDistributionChronologic state = droneStates_[index];
    return predict(state, time).first.first;
}

void Ekf::reset(){
    unique_lock<shared_mutex> lock(bigSharedMtx_);
    double altitude = droneStates_.back().first.first.position.z;

    // clear all deques
    flyControls_.clear();
    imuMeasurements_.clear();
    droneStates_.clear();

    // add default element in all queue
    flyControls_.push_back(FlyControlChronologic(FlyControl(), 0));
    imuMeasurements_.push_back(ImuMeasurementChronologic(DroneStateMeasurement(), 0));
    droneStates_.push_back(DroneStateDistributionChronologic(
        DroneStateDistribution(DroneState(xcs::CartesianVector(0,0,altitude)), arma::mat(10, 10, fill::eye)),
        0));

    IDCounter_ = 1;
}
