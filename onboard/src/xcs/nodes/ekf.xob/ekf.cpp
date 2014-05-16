#include "ekf.hpp"

#include <xcs/logging.hpp>

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
    // TODO: Instead of testing first != last in every loop use before loop simple test if dequeu have only one element?
    // bisect interval
    while (first != last ){ 
        int middle = (first + last) / 2;
        if (deque[middle].second <= time){
            first = middle;
        }
        else if (deque[middle].second > time){
            last = middle;
        }
    }

    return first;
}

// Result have to have first and last element with beginTime and endTime!!!
template <typename Deque>
Deque Ekf::findAllBetween(Deque &deque, const double &beginTime, const double &endTime){
    Deque dequeOut;

    int index = findNearest(deque, beginTime);
    if (index < 0){
        XCS_LOG_FATAL("EKF: We cannot find any flyControl with time lesser than beginTime.");
    }
    else{
        // We set same time as beginTime for the first element in dequeOut
        dequeOut.push_back(deque[index++]);
        dequeOut.back().second = beginTime;

        while (index < deque.size() && deque[index].second <= endTime){
            dequeOut.push_back(deque[index++]);
        }

        if (deque.back().second != endTime){ // Ensure that we always have last element with endTime
            dequeOut.push_back(deque.back()); 
            dequeOut.back().second = endTime;
        }
    }
    return dequeOut;
}

DroneStateDistributionChronologic Ekf::predictAndUpdateFromImu(const double &beginTime, const double &endTime){
    int index = findNearest(droneStates_, beginTime);
    
    if (index >= 0){
        DroneStateDistributionChronologic& updatedState = droneStates_[index];
        FlyControls selectedflyControls = findAllBetween(flyControls_,updatedState.second,endTime);
        Measurements selectedMeasurments = findAllBetween(measurements_, updatedState.second, endTime);
        if (selectedMeasurments.front().second == updatedState.second){ // updatedState was created from first measurement in deque -> erase first
            selectedMeasurments.pop_front();
        }

        double time = updatedState.second;
        DroneStateDistribution state = updatedState.first;
        do{
            double measurementTime = selectedMeasurments.front().second;
            double flyControlTime = time;
            do{ // predict state up to the measurement time
                double delta = std::min(selectedflyControls[1].second, measurementTime) - flyControlTime;
                state = predict(state, selectedflyControls.front().first, delta);
                if (selectedflyControls[1].second < measurementTime){
                    flyControlTime = selectedflyControls[1].second;
                    selectedflyControls.pop_front();
                }
            } while (selectedflyControls[1].second < measurementTime);
            state = updateIMU(state, selectedMeasurments.front().first);
            time = measurementTime;
            selectedMeasurments.pop_front();
        } while (time < endTime);

        printf("EKF: Computed drone state [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
            state.first.position.x, state.first.position.y, state.first.position.z,
            state.first.velocity.x, state.first.velocity.x, state.first.velocity.x,
            state.first.angles.phi, state.first.angles.theta, state.first.angles.psi,
            state.first.angularRotationPsi);
        printf("EKF: Deviation drone state [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
            state.second[0, 0], state.second[1, 1], state.second[2, 2],
            state.second[3, 3], state.second[4, 4], state.second[5, 5],
            state.second[6, 6], state.second[7, 7], state.second[8, 8],
            state.second[9, 9]);
        return DroneStateDistributionChronologic(state,endTime);
    }else{
        XCS_LOG_FATAL("EKF: We cannot find any State for update step.");
        return DroneStateDistributionChronologic();
    }
}

DroneStateDistribution Ekf::predict(const DroneStateDistribution &state, const FlyControl &flyControl, const double &delta){
    DroneStateDistribution predictedState = state;
    const CartesianVector &positionOld = state.first.position;
    const CartesianVector &velocityOld = state.first.velocity;
    const EulerianVector &anglesOld = state.first.angles;

    // predict acceleration
    // acceleration in drone frame
    double force = parameters_[0] * (1.0 + parameters_[1] * flyControl.gaz);
    double forceX = force * sin(anglesOld.phi)*cos(anglesOld.theta);
    double forceY = -force * sin(anglesOld.theta);
    // drag
    double dragX = parameters_[2] * velocityOld.x + parameters_[3] * velocityOld.x * velocityOld.x;
    double dragY = parameters_[2] * velocityOld.y + parameters_[3] * velocityOld.y * velocityOld.y;

    // drone acceleration in global frame
    CartesianVector acceleration;
    acceleration.x = (cos(anglesOld.psi)*forceX + sin(anglesOld.psi)*forceY) - dragX;
    acceleration.y = (-sin(anglesOld.psi)*forceX + cos(anglesOld.psi)*forceY) - dragY;
    acceleration.z = parameters_[8] * flyControl.gaz - parameters_[9] * velocityOld.z;

    // angular rotation speed
    EulerianVector angularRotation;
    angularRotation.phi = parameters_[4] * flyControl.roll - parameters_[5] * anglesOld.phi;
    angularRotation.theta = parameters_[4] * flyControl.pitch - parameters_[5] * anglesOld.theta;
    // angular acceleration
    angularRotation.psi = parameters_[6] * flyControl.yaw - parameters_[7] * state.first.angularRotationPsi;

    // =========== predict new state ============
    // position
    CartesianVector &position = predictedState.first.position;
    position.x += velocityOld.x*delta;
    position.y += velocityOld.y*delta;
    position.z += velocityOld.z*delta;
    // velocity
    CartesianVector &velocity = predictedState.first.velocity;
    velocity.x += acceleration.x*delta;
    velocity.y += acceleration.y*delta;
    velocity.z += acceleration.z*delta;
    // angles
    EulerianVector &angles = predictedState.first.angles;
    angles.phi += angularRotation.phi*delta;
    angles.theta += angularRotation.theta*delta;
    angles.psi += state.first.angularRotationPsi*delta;
    // angular rotation psi
    predictedState.first.angularRotationPsi += angularRotation.psi*delta;
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
    noiseTransf(3, 3) = delta*parameters_[0] * parameters_[1] * (
        cos(anglesOld.psi)*sin(anglesOld.phi)*cos(anglesOld.theta)
        -sin(anglesOld.psi)*sin(anglesOld.theta));
    noiseTransf(4, 3) = delta*parameters_[0] * parameters_[1] * (
        -sin(anglesOld.psi)*sin(anglesOld.phi)*cos(anglesOld.theta)
        -cos(anglesOld.psi)*sin(anglesOld.theta));
    // gaz
    noiseTransf(5, 3) = delta * parameters_[8];
    // phi
    noiseTransf(6, 0) = delta * parameters_[4];
    // theta
    noiseTransf(7, 1) = delta * parameters_[4];
    // rotation speed psi
    noiseTransf(9, 2) = delta * parameters_[6];

    // ======= predict state deviation ===========

    mat noise(4, 4, fill::zeros); // TODO: compute better noise
    noise(0, 0) = normalDistribution_(randomGenerator_) * 0.01;
    noise(1, 1) = normalDistribution_(randomGenerator_) * 0.01;
    noise(2, 2) = normalDistribution_(randomGenerator_) * 0.01;
    noise(3, 3) = normalDistribution_(randomGenerator_) * 0.01;

    predictedState.second = jacobian * state.second * jacobian.t() + noiseTransf * noise * noiseTransf.t();

    // ======= end predict state deviation ===========

    return predictedState;
}

DroneStateDistribution Ekf::updateIMU(const DroneStateDistribution &state, const DroneStateMeasurement &imuMeasurement){
    DroneStateDistribution updatedState;

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
    noise(0, 0) = normalDistribution_(randomGenerator_) * 0.0025;
    noise(1, 1) = normalDistribution_(randomGenerator_) * 0.01;
    noise(2, 2) = normalDistribution_(randomGenerator_) * 0.01;
    noise(3, 3) = normalDistribution_(randomGenerator_) * 0.01;
    noise(4, 4) = normalDistribution_(randomGenerator_) * 0.003;
    noise(5, 5) = normalDistribution_(randomGenerator_) * 0.003;
    noise(6, 6) = normalDistribution_(randomGenerator_) * 0.003;

    // compute kalman gain
    mat gain = state.second * measurementJacobian.t() * (measurementJacobian * state.second * measurementJacobian.t() + noise).i();

    // compute predicted measurement 
    mat predictedMeasurment(7, 1, fill::zeros);
    predictedMeasurment(0, 0) = state.first.position.z;
    predictedMeasurment(1, 0) = state.first.velocity.x * cos(state.first.angles.psi) 
        - state.first.velocity.y * sin(state.first.angles.psi);
    predictedMeasurment(2, 0) =  state.first.velocity.x * sin(state.first.angles.psi)
        + state.first.velocity.y * cos(state.first.angles.psi);
    predictedMeasurment(3, 0) = state.first.velocity.z;
    predictedMeasurment(4, 0) = state.first.angles.phi;
    predictedMeasurment(5, 0) = state.first.angles.theta;
    predictedMeasurment(6, 0) = state.first.angularRotationPsi;

    // update state
    mat newState = state.first.getMat() + gain * (imuMeasurement.getMat() - predictedMeasurment);
    updatedState.first.Mat(newState);

    // update deviation
    updatedState.second = (mat(10, 10).eye() - gain*measurementJacobian) * state.second;

    return updatedState;
}

// =========================== public functions ============================

Ekf::Ekf() : 
randomGenerator_(5){

    // add default element in all queue
    lastStateTime = 0;
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
    measurements_.push_back(MeasurementChronologic(measurement, timestamp));
    droneStates_.push_back(predictAndUpdateFromImu(timestamp, timestamp));
};

DroneState Ekf::computeState(const double &time){
    return DroneState();
}
