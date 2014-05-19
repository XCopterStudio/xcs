#include "ekf.hpp"

#include <xcs/logging.hpp>
#include <xcs/xcs_fce.hpp>

#include <cmath>

using namespace std;
using namespace xcs;
using namespace xcs::nodes::ekf;
using namespace arma;

mat DroneState::getMat() const{
    mat matrix;
    matrix << position.x << endr
        << position.y << endr
        << position.z << endr
        << velocity.x << endr
        << velocity.y << endr
        << velocity.z << endr
        << angles.phi << endr
        << angles.theta << endr
        << angles.psi << endr
        << angularRotationPsi << endr;
    return matrix;
};

void DroneState::Mat(const arma::mat &mat){
    if (mat.n_rows == 10 && mat.n_cols == 1){
        position.x = mat.at(0, 0);
        position.y = mat.at(1, 0);
        position.z = mat.at(2, 0);
        velocity.x = mat.at(3, 0);
        velocity.y = mat.at(4, 0);
        velocity.z = mat.at(5, 0);
        angles.phi = mat.at(6, 0);
        angles.theta = mat.at(7, 0);
        angles.psi = mat.at(8, 0);
        angularRotationPsi = mat.at(9, 0);
    }
    else{
        cerr << "Wrong matrix size" << endl;
    }
}

mat DroneStateMeasurement::getMat() const{
    mat measurement;
    measurement << altitude << endr
        << velocity.x << endr
        << velocity.y << endr
        << velocity.z << endr
        << angles.phi << endr
        << angles.theta << endr
        << angularRotationPsi << endr;
    return measurement;
}

template <typename Deque>
int Ekf::findNearest(Deque &deque, const double &time){ // TODO: error rewrite !!!
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
    int last = measurements_.size() - 1;

    while (first <= last){
        int middle = (first + last) / 2;
        if(measurements_[middle].first.measurementID == ID){
            return middle;
        }
        else if (measurements_[middle].first.measurementID < ID){
            first = middle+1;
        }
        else{
            last = middle - 1;
        }
    }

    return -1;
}

void Ekf::predict(DroneStateDistribution& state, const double& beginTime, const double &endTime){
    double flyControlTime = beginTime;
    int controlIndex = findNearest(flyControls_, beginTime);

    do{ // predict state up to the measurement time
        double nextControlTime = 0;
        if (controlIndex < (flyControls_.size() - 1)){
            nextControlTime = flyControls_[controlIndex + 1].second;
        }
        else{
            nextControlTime = std::numeric_limits<double>::max();
        }
        double delta = std::min(nextControlTime, endTime) - flyControlTime;
        state = predict(state, flyControls_[controlIndex].first, delta);

        if (nextControlTime < endTime){
            flyControlTime = nextControlTime;
            ++controlIndex;
        }
        else{
            break;
        }
    } while (true);
}

DroneStateDistributionChronologic Ekf::predictAndUpdateFromImu(const double &beginTime, const double &endTime){
    int index = findNearest(droneStates_, beginTime);
    
    if (index >= 0){
        DroneStateDistribution state = droneStates_[index].first;
        double time = droneStates_[index].second;

        int controlsIndex = findNearest(flyControls_,time);
        int measurementIndex = findMeasurementIndex(state.first.updateMeasurementID) + 1;

        do{
            double measurementTime = measurements_[measurementIndex].second;
            predict(state, time, measurementTime);
            /*printf("EKF: Drone measurement %i [%f,%f,%f,%f,%f,%f,%f]\n",
                measurementIndex,
                measurements_[measurementIndex].first.altitude,
                measurements_[measurementIndex].first.velocity.x,
                measurements_[measurementIndex].first.velocity.y, 
                measurements_[measurementIndex].first.velocity.z,
                measurements_[measurementIndex].first.angles.phi,
                measurements_[measurementIndex].first.angles.theta,
                measurements_[measurementIndex].first.angularRotationPsi);*/

            state = updateIMU(state, measurements_[measurementIndex].first);
            time = measurementTime;
            state.first.updateMeasurementID = measurementIndex;
            if (measurementIndex < measurements_.size() - 1){
                ++measurementIndex;
            }
            else if (time < endTime){
                predict(state, time, endTime);
                time = endTime;
            }
        } while (time < endTime);

        /*printf("EKF: Computed drone state [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
            state.first.position.x, state.first.position.y, state.first.position.z,
            state.first.velocity.x, state.first.velocity.x, state.first.velocity.x,
            state.first.angles.phi, state.first.angles.theta, state.first.angles.psi,
            state.first.angularRotationPsi);
        printf("EKF: Deviation drone state [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
            state.second(0, 0), state.second(1, 1), state.second(2, 2),
            state.second(3, 3), state.second(4, 4), state.second(5, 5),
            state.second(6, 6), state.second(7, 7), state.second(8, 8),
            state.second(9, 9));*/

        return DroneStateDistributionChronologic(state,endTime);
    }else{
        XCS_LOG_FATAL("EKF: We cannot find any State for update step.");
        return DroneStateDistributionChronologic();
    }
}

DroneStateDistribution Ekf::predict(const DroneStateDistribution &state, const FlyControl &flyControl, const double &delta){
    DroneStateDistribution newState = state;
    const CartesianVector &positionOld = state.first.position;
    const CartesianVector &velocityOld = state.first.velocity;
    const EulerianVector &anglesOld = state.first.angles;

    // predict acceleration
    // acceleration in drone frame
    double force = parameters_[0] * (1.0 + parameters_[1] * flyControl.gaz);
    double forceX = force * sin(anglesOld.phi)*cos(anglesOld.theta);
    double forceY = -force * sin(anglesOld.theta);
    //printf("EKF: Force [%f,%f] \n",forceX,forceY);
    // drag
    double dragX = parameters_[2] * velocityOld.x + parameters_[3] * velocityOld.x * velocityOld.x;
    double dragY = parameters_[2] * velocityOld.y + parameters_[3] * velocityOld.y * velocityOld.y;
    //printf("EKF: Drag[%f,%f] \n", dragX,dragY);
    // drone acceleration in global frame
    CartesianVector acceleration;
    acceleration.x = (cos(anglesOld.psi)*forceX + sin(anglesOld.psi)*forceY) - dragX;
    acceleration.y = (-sin(anglesOld.psi)*forceX + cos(anglesOld.psi)*forceY) - dragY;
    acceleration.z = parameters_[8] * flyControl.gaz - parameters_[9] * velocityOld.z;
    //printf("EKF: Acceleration [%f,%f,%f] \n", acceleration.x, acceleration.y, acceleration.z);

    // angular rotation speed
    EulerianVector angularRotation;
    angularRotation.phi = parameters_[4] * flyControl.roll - parameters_[5] * anglesOld.phi;
    angularRotation.theta = parameters_[4] * flyControl.pitch - parameters_[5] * anglesOld.theta;
    // angular acceleration
    angularRotation.psi = parameters_[6] * flyControl.yaw - parameters_[7] * state.first.angularRotationPsi;
    //printf("EKF: Rotation [%f,%f,%f] \n", angularRotation.phi, angularRotation.theta, angularRotation.psi);

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
    // angles
    EulerianVector &angles = newState.first.angles;
    angles.phi = xcs::normAngle(angles.phi + angularRotation.phi*delta);
    angles.theta = xcs::normAngle(angles.theta + angularRotation.theta*delta);
    angles.psi = xcs::normAngle(angles.psi + state.first.angularRotationPsi*delta);
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
    noise(0, 0) = normalDistribution_(randomGenerator_) * 0.01;
    noise(1, 1) = normalDistribution_(randomGenerator_) * 0.01;
    noise(2, 2) = normalDistribution_(randomGenerator_) * 0.01;
    noise(3, 3) = normalDistribution_(randomGenerator_) * 0.01;

    newState.second = jacobian * state.second * jacobian.t() + noiseTransf * noise * noiseTransf.t();

    // ======= end predict state deviation ===========

    //printf("EKF: delta %f\n", delta);
    printf("EKF: Computed drone predictedState [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
        newState.first.position.x, newState.first.position.y, newState.first.position.z,
        newState.first.velocity.x, newState.first.velocity.x, newState.first.velocity.x,
        newState.first.angles.phi, newState.first.angles.theta, newState.first.angles.psi,
        newState.first.angularRotationPsi);
    ////printf("EKF: Deviation drone predictedState (%f,%f,%f,%f,%f,%f,%f,%f,%f,%f)\n",
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
    measurementJacobian(1, 4) = -sin(state.first.angles.psi);
    measurementJacobian(1, 8) = -state.first.velocity.x*sin(state.first.angles.psi) 
        - state.first.velocity.y*cos(state.first.angles.psi);
    // acceleration y
    measurementJacobian(2, 3) = sin(state.first.angles.psi);
    measurementJacobian(2, 4) = cos(state.first.angles.psi);
    measurementJacobian(2, 8) = state.first.velocity.x*cos(state.first.angles.psi)
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
    noise(0, 0) = normalDistribution_(randomGenerator_) * 0.0025; // altitude variance 
    noise(1, 1) = normalDistribution_(randomGenerator_) * 0.01; // velocity x
    noise(2, 2) = normalDistribution_(randomGenerator_) * 0.01; // velocity y
    noise(3, 3) = normalDistribution_(randomGenerator_) * 0.01; // velocity z
    noise(4, 4) = normalDistribution_(randomGenerator_) * 0.122; // phi 
    noise(5, 5) = normalDistribution_(randomGenerator_) * 0.122; // theta
    noise(6, 6) = normalDistribution_(randomGenerator_) * 0.122; // angular velocity psi

    // compute kalman gain
    mat gain = state.second * measurementJacobian.t() * (measurementJacobian * state.second * measurementJacobian.t() + noise).i();

    // compute predicted measurement 
    mat predictedMeasurement(7, 1);
    predictedMeasurement(0, 0) = state.first.position.z;
    predictedMeasurement(1, 0) = state.first.velocity.x * cos(state.first.angles.psi) 
        - state.first.velocity.y * sin(state.first.angles.psi);
    predictedMeasurement(2, 0) =  state.first.velocity.x * sin(state.first.angles.psi)
        + state.first.velocity.y * cos(state.first.angles.psi);
    predictedMeasurement(3, 0) = state.first.velocity.z;
    predictedMeasurement(4, 0) = state.first.angles.phi;
    predictedMeasurement(5, 0) = state.first.angles.theta;
    predictedMeasurement(6, 0) = state.first.angularRotationPsi;

    // update state
    DroneStateDistribution newState;
    mat newStateMean = state.first.getMat() + gain * (imuMeasurement.getMat() - predictedMeasurement);
    newState.first.Mat(newStateMean);

    // update deviation
    newState.second = (mat(10, 10).eye() - gain*measurementJacobian) * state.second;

    printf("EKF: Computed drone updatedState [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
        newState.first.position.x, newState.first.position.y, newState.first.position.z,
        newState.first.velocity.x, newState.first.velocity.x, newState.first.velocity.x,
        newState.first.angles.phi, newState.first.angles.theta, newState.first.angles.psi,
        newState.first.angularRotationPsi);

    return newState;
}

// =========================== public functions ============================

Ekf::Ekf() : 
randomGenerator_(5){

    IDCounter_ = 1;

    // add default element in all queue
    flyControls_.push_back(FlyControlChronologic(FlyControl(),0));
    measurements_.push_back(MeasurementChronologic(DroneStateMeasurement(),0));
    droneStates_.push_back(DroneStateDistributionChronologic(
        DroneStateDistribution(DroneState(), arma::mat(10, 10, fill::eye)),
        0));

    // Constants taken from tum_ardrone 
    parameters_[0] = 0.58;
    parameters_[1] = 0; // TODO: compute
    parameters_[2] = 17.8;
    parameters_[3] = 0; // TODO: compute
    parameters_[4] = 10;
    parameters_[5] = 35;
    parameters_[6] = 10;
    parameters_[7] = 25;
    parameters_[8] = 1.4;
    parameters_[9] = 1.0;

}

void Ekf::flyControl(const FlyControl &flyControl, const double &timestamp){
    XCS_LOG_INFO("Inserted new flyControl with timestamp: " << timestamp);
    flyControls_.push_back(FlyControlChronologic(flyControl, timestamp));
};

void Ekf::measurement(const DroneStateMeasurement &measurement, const double &timestamp){
    XCS_LOG_INFO("Inserted new imu measurement with timestamp: " << timestamp);
    MeasurementChronologic copyMeasurement(measurement, timestamp);
    copyMeasurement.first.measurementID = IDCounter_++;
    measurements_.push_back(copyMeasurement);
    droneStates_.push_back(predictAndUpdateFromImu(timestamp, timestamp));
};

DroneState Ekf::computeState(const double &time){
    return DroneState();
}
