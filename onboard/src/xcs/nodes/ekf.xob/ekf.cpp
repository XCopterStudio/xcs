#include "ekf.hpp"

#include <cmath>

using namespace std;
using namespace xcs::nodes::ekf;
using namespace xcs::nodes::xci;
using namespace arma;

mat DroneState::getMat() const{
    mat matrix(10, 1);
    matrix[1, 1] = position.x;
    matrix[2, 1] = position.y;
    matrix[3, 1] = position.z;
    matrix[4, 1] = velocity.x;
    matrix[5, 1] = velocity.y;
    matrix[6, 1] = velocity.z;
    matrix[7, 1] = angles.phi;
    matrix[8, 1] = angles.theta;
    matrix[9, 1] = angles.psi;
    matrix[10, 1] = angularRotationPsi;
    return matrix;
};

void DroneState::Mat(const arma::mat &mat){
    if (mat.n_rows == 10 && mat.n_cols == 1){
        position.x = mat[1, 1];
        position.y = mat[2, 1];
        position.z = mat[3, 1];
        velocity.x = mat[4, 1];
        velocity.y = mat[5, 1];
        velocity.z = mat[6, 1];
        angles.phi = mat[7, 1];
        angles.theta = mat[8, 1];
        angles.psi = mat[9, 1];
        angularRotationPsi = mat[10, 1];
    }
    else{
        cerr << "Wrong matrix size" << endl;
    }
}

mat DroneStateMeasurement::getMat() const{
    mat measurement(7, 1);
    measurement[1, 1] = altitude;
    measurement[2, 1] = velocity.x;
    measurement[3, 1] = velocity.y;
    measurement[4, 1] = velocity.z;
    measurement[5, 1] = angles.phi;
    measurement[6, 1] = angles.theta;
    measurement[7, 1] = angularRotationPsi;
    return measurement;
}

DroneStateDistribution Ekf::predict(const DroneStateDistribution &state, const FlyParam &flyparam, const double &delta){
    DroneStateDistribution predictedState = state;
    const CartesianVector &positionOld = state.first.position;
    const CartesianVector &velocityOld = state.first.velocity;
    const EulerianVector &anglesOld = state.first.angles;

    // predict acceleration
    // acceleration in drone frame
    double force = parameters_[0] * (1.0 + parameters_[1] * flyparam.gaz);
    double forceX = force * sin(anglesOld.phi)*cos(anglesOld.theta);
    double forceY = -force * sin(anglesOld.theta);
    // friction
    double dragX = parameters_[2] * velocityOld.x + parameters_[3] * velocityOld.x * velocityOld.x;
    double dragY = parameters_[2] * velocityOld.y + parameters_[3] * velocityOld.y * velocityOld.y;

    // drone acceleration in global frame
    CartesianVector acceleration;
    acceleration.x = (cos(anglesOld.psi)*forceX + sin(anglesOld.psi)*forceY) - dragX;
    acceleration.y = (-sin(anglesOld.psi)*forceX + cos(anglesOld.psi)*forceY) - dragY;
    acceleration.z = parameters_[8] * flyparam.gaz - parameters_[9] * velocityOld.z;

    // angular rotation speed
    EulerianVector angularRotation;
    angularRotation.phi = parameters_[4] * flyparam.roll - parameters_[5] * anglesOld.phi;
    angularRotation.theta = parameters_[4] * flyparam.pitch - parameters_[5] * anglesOld.theta;
    // angular acceleration
    angularRotation.psi = parameters_[6] * flyparam.yaw - parameters_[7] * state.first.angularRotationPsi;

    // =========== predict new state ============
    // position
    CartesianVector &position = predictedState.first.position;
    position.x += velocityOld.x*delta + acceleration.x*delta*delta / 2.0;
    position.y += velocityOld.y*delta + acceleration.y*delta*delta / 2.0;
    position.z += velocityOld.z*delta + acceleration.z*delta*delta / 2.0;
    // velocity
    CartesianVector &velocity = predictedState.first.velocity;
    velocity.x += delta*acceleration.x;
    velocity.y += delta*acceleration.y;
    velocity.z += delta*acceleration.z;
    // angles
    EulerianVector &angles = predictedState.first.angles;
    angles.phi += angularRotation.phi*delta;
    angles.theta += angularRotation.theta*delta;
    angles.psi += state.first.angularRotationPsi*delta + angularRotation.psi*delta*delta / 2.0;
    // angular rotation psi
    predictedState.first.angularRotationPsi += angularRotation.psi*delta;
    // =========== end predict new state ========

    // create jacobian matrix 
    mat jacobian(10, 10, fill::zeros);
    // position x
    jacobian[1, 1] = 1;
    jacobian[4, 1] = delta;
    // position y
    jacobian[2, 2] = 1;
    jacobian[5, 2] = delta;
    // position z
    jacobian[3, 3] = 1;
    jacobian[6, 3] = delta;
    // velocity x
    jacobian[4, 4] = 1 - parameters_[2] * delta - parameters_[3] * 2 * delta * velocityOld.x;
    jacobian[7, 4] = delta*force * (
        cos(anglesOld.phi) * cos(anglesOld.psi) * cos(anglesOld.theta) 
        );
    jacobian[8, 4] = delta*force * (
        -sin(anglesOld.phi) * cos(anglesOld.psi) * sin(anglesOld.theta)
        -sin(anglesOld.psi) * cos(anglesOld.theta)
        );
    jacobian[9, 4] = delta*force * (
        -sin(anglesOld.phi) * sin(anglesOld.psi) * cos(anglesOld.theta)
        -cos(anglesOld.psi) * sin(anglesOld.theta)
        );
    // velocity y
    jacobian[5, 5] = 1 - parameters_[2] * delta - parameters_[3] * 2 * delta * velocityOld.y;
    jacobian[7, 5] = delta*force * (
        -cos(anglesOld.phi) * sin(anglesOld.psi) * cos(anglesOld.theta)
        );
    jacobian[8, 5] = delta*force * (
        sin(anglesOld.phi) * sin(anglesOld.psi) * sin(anglesOld.theta)
        - cos(anglesOld.psi) * cos(anglesOld.theta)
        );
    jacobian[9, 5] = delta*force * (
        -sin(anglesOld.phi) * cos(anglesOld.psi) * cos(anglesOld.theta)
        + sin(anglesOld.psi) * sin(anglesOld.theta)
        );
    // velocity z
    jacobian[6, 6] = 1 - delta*parameters_[9];
    // angle phi
    jacobian[7, 7] = 1 - delta*parameters_[5];
    // angle theta
    jacobian[8, 8] = 1 - delta*parameters_[5];
    // angle psi
    jacobian[9, 9] = 1;
    jacobian[10, 9] = delta;
    // rotation speed psi
    jacobian[10, 10] = 1 - delta*parameters_[7];

    // normal noise
    mat noiseTransf(4, 10, fill::zeros);
    // gaz
    noiseTransf[4, 6] = delta * parameters_[8];
    // phi
    noiseTransf[1, 7] = delta * parameters_[4];
    // theta
    noiseTransf[2, 8] = delta * parameters_[4];
    // rotation speed psi
    noiseTransf[3, 10] = delta * parameters_[6];

    // ======= predict state deviation ===========

    mat noise(4, 4, fill::zeros);
    noise[1, 1] = normalDistribution_(randomGenerator_) * flyparam.roll * flyparam.roll;
    noise[2, 2] = normalDistribution_(randomGenerator_) * flyparam.pitch * flyparam.pitch;
    noise[3, 3] = normalDistribution_(randomGenerator_) * flyparam.yaw * flyparam.yaw;
    noise[4, 4] = normalDistribution_(randomGenerator_) * flyparam.gaz * flyparam.gaz;

    predictedState.second = jacobian * state.second * jacobian.t() + noiseTransf * noise * noiseTransf.t();

    // ======= end predict state deviation ===========

    return predictedState;
}

DroneStateDistribution Ekf::updateIMU(const DroneStateDistribution &state, const DroneStateMeasurement &imuMeasurement){
    DroneStateDistribution updatedState;

    // create jacobian from measurement function
    mat measurementJacobian(7, 10, fill::zeros);
    // altitude
    measurementJacobian[1, 3] = 1;
    // acceleration x
    measurementJacobian[2, 4] = cos(state.first.angles.psi);
    measurementJacobian[2, 5] = -sin(state.first.angles.psi);
    measurementJacobian[2, 9] = -state.first.velocity.x*sin(state.first.angles.psi) 
        - state.first.velocity.y*cos(state.first.angles.psi);
    // acceleration y
    measurementJacobian[3, 4] = sin(state.first.angles.psi);
    measurementJacobian[3, 5] = cos(state.first.angles.psi);
    measurementJacobian[3, 9] = state.first.velocity.x*cos(state.first.angles.psi)
        - state.first.velocity.y*sin(state.first.angles.psi);
    // acceleration z
    measurementJacobian[4, 6] = 1;
    // phi
    measurementJacobian[5, 7] = 1;
    // theta
    measurementJacobian[6, 8] = 1;
    // psi
    measurementJacobian[7, 10] = 1;

    // additional noise 
    mat noise(7, 7, fill::zeros);
    noise[1, 1] = normalDistribution_(randomGenerator_) * imuMeasurement.altitude * imuMeasurement.altitude;
    noise[2, 2] = normalDistribution_(randomGenerator_) * imuMeasurement.velocity.x * imuMeasurement.velocity.x;
    noise[3, 3] = normalDistribution_(randomGenerator_) * imuMeasurement.velocity.y * imuMeasurement.velocity.y;
    noise[4, 4] = normalDistribution_(randomGenerator_) * imuMeasurement.velocity.z * imuMeasurement.velocity.z;
    noise[5, 5] = normalDistribution_(randomGenerator_) * imuMeasurement.angles.phi * imuMeasurement.angles.phi;
    noise[6, 6] = normalDistribution_(randomGenerator_) * imuMeasurement.angles.theta * imuMeasurement.angles.theta;
    noise[7, 7] = normalDistribution_(randomGenerator_) * imuMeasurement.angularRotationPsi * imuMeasurement.angularRotationPsi;

    // compute kalman gain
    mat gain(10, 7);
    gain = state.second * measurementJacobian.t() * (measurementJacobian * state.second * measurementJacobian.t() + noise).i();

    // compute predicted measurement 
    mat predictedMeasurment(7, 1, fill::zeros);
    predictedMeasurment[1, 1] = state.first.position.z;
    predictedMeasurment[2, 1] = state.first.velocity.x * cos(state.first.angles.psi) 
        - state.first.velocity.y * sin(state.first.angles.psi);
    predictedMeasurment[3, 1] = state.first.velocity.x * sin(state.first.angles.psi)
        + state.first.velocity.y * cos(state.first.angles.psi);
    predictedMeasurment[4, 1] = state.first.velocity.z;
    predictedMeasurment[5, 1] = state.first.angles.phi;
    predictedMeasurment[6, 1] = state.first.angles.theta;
    predictedMeasurment[7, 1] = state.first.angularRotationPsi;

    // update state
    mat newState(10, 1);
    newState = state.first.getMat() + gain * (imuMeasurement.getMat() - predictedMeasurment);
    updatedState.first.Mat(newState);

    // update deviation
    updatedState.second = (mat(10, 10).eye() - gain*measurementJacobian) * state.second;

    return updatedState;
}

// =========================== public functions ============================

Ekf::Ekf() : 
randomGenerator_(5){

}

void flyParam(const FlyParam &flyParam, const long int &timestamp){

};

void measurement(const DroneStateMeasurement &measurement, const long int &timestamp){

};