#include "ekf.hpp"

#include <xcs/logging.hpp>
#include <xcs/xcs_fce.hpp>

#include <cmath>

using namespace std;
using namespace xcs;
using namespace xcs::nodes::localization;
using namespace arma;

template <typename Deque>
void Ekf::clearUpToTime(Deque &deque, const double &time){
    int index = findNearest(deque, time);
    for (int i = 0; i < index; ++i){
        deque.pop_front();
    }
}

template <typename Deque>
int Ekf::findNearest(Deque &deque, const double &time){ 
    if (deque.front().second > time){ // We do not have any elements in deque which time is lesser than input time
        return -1;
    }

    int first = 0;
    int last = deque.size() - 1;
    
    if (deque[last].second <= time){
        return last;
    }

    while (first <= last ){ 
        int middle = (first + last) / 2;
        if (deque[middle].second <= time && deque[middle + 1].second > time){
            return middle;
        }
        else if (deque[middle].second < time){
            first = middle + 1;
        }
        else{
            last = middle - 1;
        }
    }

    return -1;
}

int Ekf::findMeasurementIndex(const int &ID){
    int first = 0;
    int last = imuMeasurements_.size() - 1;

    while (first <= last){
        int middle = (first + last) / 2;
        if(imuMeasurements_[middle].first.measurementID == ID){
            return middle;
        }
        else if (imuMeasurements_[middle].first.measurementID < ID){
            first = middle+1;
        }
        else{
            last = middle - 1;
        }
    }

    return -1;
}

DroneStateDistributionChronologic Ekf::predict(const DroneStateDistributionChronologic& state, const double &endTime){
    DroneStateDistributionChronologic newState = state;
    double flyControlTime = state.second;
    int controlIndex = findNearest(flyControls_, flyControlTime);

    do{ // predict state up to the measurement time
        double nextControlTime = 0;
        if (controlIndex < (flyControls_.size() - 1)){
            nextControlTime = flyControls_[controlIndex + 1].second;
        }
        else{
            nextControlTime = std::numeric_limits<double>::max();
        }
        double delta = std::min(nextControlTime, endTime) - flyControlTime;
        newState.first = predict(newState.first, flyControls_[controlIndex].first, delta);

        
       flyControlTime = nextControlTime;
        ++controlIndex;
    } while (flyControlTime < endTime);

    newState.second = endTime;
    return newState;
}

DroneStateDistributionChronologic Ekf::predictAndUpdateFromImu(const DroneStateDistributionChronologic& state, const double &endTime, bool saveInterResults){
    DroneStateDistributionChronologic newState = state;

    int controlsIndex = findNearest(flyControls_, newState.second);
    int measurementIndex = findMeasurementIndex(state.first.first.updateMeasurementID) + 1;

    do{
        if (measurementIndex < imuMeasurements_.size()){
            double measurementTime = imuMeasurements_[measurementIndex].second;
            newState = predict(newState, measurementTime);
        }
        else{
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

        newState.first = updateIMU(newState.first, imuMeasurements_[measurementIndex++].first);
        if (saveInterResults){
            droneStates_.push_back(newState);
        }
    } while (newState.second < endTime);

    return newState;
}

DroneStateDistribution Ekf::predict(const DroneStateDistribution &state, const FlyControl &flyControl, const double &delta){
    DroneStateDistribution newState = state;
    const CartesianVector &positionOld = state.first.position;
    const CartesianVector &velocityOld = state.first.velocity;
    const EulerianVector &anglesOld = state.first.angles;

    //M: printf("EKF: flyControl [%f,%f,%f,%f] \n", flyControl.roll, flyControl.pitch, flyControl.yaw, flyControl.gaz);

    // predict acceleration
    // acceleration in drone frame
    double force = parameters_[0] * (1.0 + parameters_[1] * flyControl.gaz);
    double forceX = force * sin(anglesOld.phi)*cos(anglesOld.theta);
    double forceY = -force * sin(anglesOld.theta);
    ////M: printf("EKF: Force [%f,%f] \n",forceX,forceY);
    // drag
    double dragX = parameters_[2] * velocityOld.x + parameters_[3] * velocityOld.x * velocityOld.x;
    double dragY = parameters_[2] * velocityOld.y + parameters_[3] * velocityOld.y * velocityOld.y;
    ////M: printf("EKF: Drag[%f,%f] \n", dragX,dragY);
    // drone acceleration in global frame
    CartesianVector acceleration;
    acceleration.x = (cos(anglesOld.psi)*forceX + sin(anglesOld.psi)*forceY) - dragX;
    acceleration.y = (-sin(anglesOld.psi)*forceX + cos(anglesOld.psi)*forceY) - dragY;
    acceleration.z = parameters_[8] * flyControl.gaz - parameters_[9] * velocityOld.z;
    ////M: printf("EKF: Acceleration [%f,%f,%f] \n", acceleration.x, acceleration.y, acceleration.z);

    // angular rotation speed
    EulerianVector angularRotation;
    angularRotation.phi = parameters_[4] * flyControl.roll - parameters_[5] * anglesOld.phi;
    angularRotation.theta = parameters_[4] * flyControl.pitch - parameters_[5] * anglesOld.theta;
    // angular acceleration
    angularRotation.psi = parameters_[6] * flyControl.yaw - parameters_[7] * state.first.angularRotationPsi;
    ////M: printf("EKF: Rotation [%f,%f,%f] \n", angularRotation.phi, angularRotation.theta, angularRotation.psi);

    // =========== predict new state ============
    // position
    CartesianVector &position = newState.first.position;
    position.x += velocityOld.x*delta;
    position.y += velocityOld.y*delta;
    position.z += velocityOld.z*delta;
    // velocity
    CartesianVector &velocity = newState.first.velocity;
    velocity.x += acceleration.x*delta;
    velocity.y += acceleration.y*delta;
    velocity.z += acceleration.z*delta;
    ////M: printf("EKF: velocity[%f,%f,%f]\n", velocity.x, velocity.y, velocity.z);
    // angles
    EulerianVector &angles = newState.first.angles;
    angles.phi = xcs::normAngle(angles.phi + angularRotation.phi*delta);
    angles.theta = xcs::normAngle(angles.theta + angularRotation.theta*delta);
    angles.psi = xcs::normAngle(angles.psi + state.first.angularRotationPsi *delta);
    // angular rotation psi
    newState.first.angularRotationPsi += angularRotation.psi*delta;
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
    jacobian(3, 3) = 1 - parameters_[2] * delta - parameters_[3] * 2 * delta * velocityOld.x;
    jacobian(3, 6) = delta*force * (
        cos(anglesOld.phi) * cos(anglesOld.psi) * cos(anglesOld.theta) 
        );
    jacobian(3, 7) = delta*force * (
        -sin(anglesOld.phi) * cos(anglesOld.psi) * sin(anglesOld.theta)
        -sin(anglesOld.psi) * cos(anglesOld.theta)
        );
    jacobian(3, 8) = delta*force * (
        -sin(anglesOld.phi) * sin(anglesOld.psi) * cos(anglesOld.theta)
        -cos(anglesOld.psi) * sin(anglesOld.theta)
        );
    // velocity y
    jacobian(4, 4) = 1 - parameters_[2] * delta - parameters_[3] * 2 * delta * velocityOld.y;
    jacobian(4, 6) = delta*force * (
        -cos(anglesOld.phi) * sin(anglesOld.psi) * cos(anglesOld.theta)
        );
    jacobian(4, 7) = delta*force * (
        sin(anglesOld.phi) * sin(anglesOld.psi) * sin(anglesOld.theta)
        - cos(anglesOld.psi) * cos(anglesOld.theta)
        );
    jacobian(4, 8) = delta*force * (
        -sin(anglesOld.phi) * cos(anglesOld.psi) * cos(anglesOld.theta)
        + sin(anglesOld.psi) * sin(anglesOld.theta)
        );
    // velocity z
    jacobian(5, 5) = 1 - delta*parameters_[9];
    // angle phi
    jacobian(6, 6) = 1 - delta*parameters_[5];
    // angle theta
    jacobian(7, 7) = 1 - delta*parameters_[5];
    // angle psi
    jacobian(8, 8) = 1;
    jacobian(8, 9) = delta;
    // rotation speed psi
    jacobian(9, 9) = 1 - delta*parameters_[7];

    // normal noise
    mat noiseTransf(10, 4, fill::zeros);
    // phi
    noiseTransf(6, 0) = delta * parameters_[4];
    // theta
    noiseTransf(7, 1) = delta * parameters_[4];
    // rotation speed psi
    noiseTransf(9, 2) = delta * parameters_[6];
    // gaz
    noiseTransf(3, 3) = delta*parameters_[0] * parameters_[1] * (
        cos(anglesOld.psi)*sin(anglesOld.phi)*cos(anglesOld.theta)
        - sin(anglesOld.psi)*sin(anglesOld.theta));
    noiseTransf(4, 3) = delta*parameters_[0] * parameters_[1] * (
        -sin(anglesOld.psi)*sin(anglesOld.phi)*cos(anglesOld.theta)
        - cos(anglesOld.psi)*sin(anglesOld.theta));
    noiseTransf(5, 3) = delta * parameters_[8];

    // ======= predict state deviation ===========
    mat noise(4, 4,fill::zeros); // TODO: compute better noise
    /*noise(0, 0) = normalDistribution_(randomGenerator_) * 16 * delta * delta;
    noise(1, 1) = normalDistribution_(randomGenerator_) * 16 * delta * delta;
    noise(2, 2) = normalDistribution_(randomGenerator_) * 16 * delta * delta;
    noise(3, 3) = normalDistribution_(randomGenerator_) * 16 * delta * delta;*/

    noise(0, 0) = 16 * delta;
    noise(1, 1) = 16 * delta;
    noise(2, 2) = 16 * delta;
    noise(3, 3) = 16 * delta;

    newState.second = jacobian * state.second * jacobian.t() + noiseTransf * noise * noiseTransf.t();

    // ======= end predict state deviation ===========

    ////M: printf("EKF: delta %f\n", delta);
    //M: printf("EKF: Computed drone predictedState [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
//        newState.first.position.x, newState.first.position.y, newState.first.position.z,
//        newState.first.velocity.x, newState.first.velocity.y, newState.first.velocity.z,
//        newState.first.angles.phi, newState.first.angles.theta, newState.first.angles.psi,
//        newState.first.angularRotationPsi);
    //////M: printf("EKF: Deviation drone predictedState (%f,%f,%f,%f,%f,%f,%f,%f,%f,%f)\n",
    //    newState.second(0, 0), newState.second(1, 1), newState.second(2, 2),
    //    newState.second(3, 3), newState.second(4, 4), newState.second(5, 5),
    //    newState.second(6, 6), newState.second(7, 7), newState.second(8, 8),
    //    newState.second(9, 9));

    return newState;
}

DroneStateDistribution Ekf::updateIMU(const DroneStateDistribution &state, const DroneStateMeasurement &imuMeasurement){
    // create jacobian from measurement function
    mat measurementJacobian(7, 10, fill::zeros);
    // altitude
    measurementJacobian(0, 2) = 1;
    // acceleration x
    measurementJacobian(1, 3) = cos(state.first.angles.psi);
    measurementJacobian(1, 4) = sin(state.first.angles.psi);
    measurementJacobian(1, 8) = -state.first.velocity.x*sin(state.first.angles.psi) 
        + state.first.velocity.y*cos(state.first.angles.psi);
    // acceleration y
    measurementJacobian(2, 3) = -sin(state.first.angles.psi);
    measurementJacobian(2, 4) = cos(state.first.angles.psi);
    measurementJacobian(2, 8) = -state.first.velocity.x*cos(state.first.angles.psi)
        - state.first.velocity.y*sin(state.first.angles.psi);
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
    noise(0, 0) = 0.0004; // altitude variance 
    noise(1, 1) = 0.000025; // velocity x
    noise(2, 2) = 0.000025; // velocity y
    noise(3, 3) = 0.01; // velocity z
    noise(4, 4) = 0.0003; // phi 
    noise(5, 5) = 0.0003; // theta
    noise(6, 6) = 0.01; // angular velocity psi

    // compute kalman gain
    mat gain = state.second * measurementJacobian.t() * (measurementJacobian * state.second * measurementJacobian.t() + noise).i();

    // compute predicted measurement 
    mat predictedMeasurement(7, 1);
    predictedMeasurement(0, 0) = state.first.position.z;
    predictedMeasurement(1, 0) = state.first.velocity.x * cos(state.first.angles.psi) 
        + state.first.velocity.y * sin(state.first.angles.psi);
    predictedMeasurement(2, 0) =  - state.first.velocity.x * sin(state.first.angles.psi)
        + state.first.velocity.y * cos(state.first.angles.psi);
    predictedMeasurement(3, 0) = state.first.velocity.z;
    predictedMeasurement(4, 0) = state.first.angles.phi;
    predictedMeasurement(5, 0) = state.first.angles.theta;
    predictedMeasurement(6, 0) = state.first.angularRotationPsi;

    // update state
    mat newStateMean = static_cast<mat>(state.first) + gain * (static_cast<mat>(imuMeasurement) - predictedMeasurement);

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
    

    //M: printf("EKF: Computed drone updatedState [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
//        newState.first.position.x, newState.first.position.y, newState.first.position.z,
//        newState.first.velocity.x, newState.first.velocity.y, newState.first.velocity.z,
//        newState.first.angles.phi, newState.first.angles.theta, newState.first.angles.psi,
//        newState.first.angularRotationPsi);

    return newState;
}

DroneStateDistribution Ekf::updateCam(const DroneStateDistribution &state, const CameraMeasurement &camMeasurement){
    mat measurementJacobian(6, 10, fill::zeros);
    measurementJacobian(0, 0) = 1; // x
    measurementJacobian(1, 1) = 1; // y
    measurementJacobian(2, 2) = 1; // z
    measurementJacobian(3, 6) = 1; // phi
    measurementJacobian(4, 7) = 1; // theta
    measurementJacobian(5, 8) = 1; // psi

    mat noise(6, 6, fill::zeros);
    noise(0, 0) = 0.01; // x 
    noise(1, 1) = 0.01; // y
    noise(2, 2) = 0.01; // z
    noise(3, 3) = 0.122; // phi
    noise(4, 4) = 0.122; // theta 
    noise(5, 5) = 0.122; // psi

    // compute kalman gain
    mat gain = state.second * measurementJacobian.t() * (measurementJacobian * state.second * measurementJacobian.t() + noise).i();

    // compute predicted measurement 
    mat predictedMeasurement(6, 1);
    predictedMeasurement(0, 0) = state.first.position.x;
    predictedMeasurement(1, 0) = state.first.velocity.y;
    predictedMeasurement(2, 0) = state.first.velocity.z;
    predictedMeasurement(3, 0) = state.first.angles.phi;
    predictedMeasurement(4, 0) = state.first.angles.theta;
    predictedMeasurement(5, 0) = state.first.angles.psi;

    // update state
    DroneStateDistribution newState;
    mat newStateMean = static_cast<mat>(state.first) + gain * (static_cast<mat>(camMeasurement) - predictedMeasurement);
    newState.first = newStateMean;

    // update deviation
    newState.second = (mat(10, 10).eye() - gain*measurementJacobian) * state.second;

    //M: printf("EKF: Computed drone updatedState [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
//        newState.first.position.x, newState.first.position.y, newState.first.position.z,
//        newState.first.velocity.x, newState.first.velocity.y, newState.first.velocity.z,
//        newState.first.angles.phi, newState.first.angles.theta, newState.first.angles.psi,
//        newState.first.angularRotationPsi);

    return newState;
}

// =========================== public functions ============================

Ekf::Ekf() : 
randomGenerator_(5){

    IDCounter_ = 1;

    // add default element in all queue
    flyControls_.push_back(FlyControlChronologic(FlyControl(),0));
    imuMeasurements_.push_back(ImuMeasurementChronologic(DroneStateMeasurement(),0));
    droneStates_.push_back(DroneStateDistributionChronologic(
        DroneStateDistribution(DroneState(), arma::mat(10, 10, fill::eye)),
        0));

    // Constants taken from tum_ardrone 
    //parameters_[0] = 0.58;
    //parameters_[1] = 0; // TODO: compute
    //parameters_[2] = 17.8;
    //parameters_[3] = 0; // TODO: compute
    //parameters_[4] = 10;
    //parameters_[5] = 35;
    //parameters_[6] = 10;
    //parameters_[7] = 25;
    //parameters_[8] = 1.4;
    //parameters_[9] = 1.0;

    parameters_[0] = 10.32;
    parameters_[1] = 0; // TODO: compute
    parameters_[2] = 0.58;
    parameters_[3] = 0; // TODO: compute
    parameters_[4] = 6.108;
    parameters_[5] = 0.175;
    parameters_[6] = 4.363;
    parameters_[7] = 0.175;
    parameters_[8] = 1.4;
    parameters_[9] = 1.4;

}

void Ekf::clearUpToTime(const double timestamp){
    clearUpToTime(droneStates_, timestamp);
    clearUpToTime(flyControls_, timestamp);
    clearUpToTime(imuMeasurements_, timestamp);
}

void Ekf::flyControl(const FlyControl flyControl, const double timestamp){
    XCS_LOG_INFO("Inserted new flyControl with timestamp: " << timestamp);
    flyControls_.push_back(FlyControlChronologic(flyControl, timestamp));
};

void Ekf::measurementImu(const DroneStateMeasurement measurement, const double timestamp){
    //XCS_LOG_INFO("Inserted new imu measurement with timestamp: " << timestamp);
    ImuMeasurementChronologic copyMeasurement(measurement, timestamp);
    copyMeasurement.first.measurementID = IDCounter_++;
    imuMeasurements_.push_back(copyMeasurement);
    int index = findNearest(droneStates_, timestamp); // TODO: check -1 when findNearest cannot find any state which we can update
    droneStates_.push_back(predictAndUpdateFromImu(droneStates_[index], timestamp));
};

void Ekf::measurementCam(const CameraMeasurement measurement, const double timestamp){  // TODO:
    XCS_LOG_INFO("Inserted new camera measurement with timestamp: " << timestamp);
    int index = findNearest(droneStates_, timestamp);
    DroneStateDistributionChronologic newState = droneStates_[index];
    //delete all old droneStates
    droneStates_.clear();

    // create new droneStates up to the time of last imuMeasurements
    newState = predict(newState, timestamp);
    newState.first = updateCam(newState.first, measurement);
    // save cam update
    droneStates_.push_back(newState);
    // update up to the last imu measurements
    newState = predictAndUpdateFromImu(newState, imuMeasurements_.back().second,true);

    // clear old imuMeasurements and flyControls
    clearUpToTime(imuMeasurements_, timestamp);
    clearUpToTime(flyControls_, timestamp);
}

DroneState Ekf::computeState(const double time){
    int index = findNearest(droneStates_, time);
    DroneStateDistributionChronologic state = droneStates_[index];
    return predict(state, time).first.first;
}
