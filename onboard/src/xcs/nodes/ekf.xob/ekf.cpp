#include "ekf.hpp"

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
    jacobian(0, 0) = 1;
    jacobian(3, 0) = delta;
    // position y
    jacobian(1, 1) = 1;
    jacobian(4, 1) = delta;
    // position z
    jacobian(2, 2) = 1;
    jacobian(5, 2) = delta;
    // velocity x
    jacobian(3, 3) = 1 - parameters_[2] * delta - parameters_[3] * 2 * delta * velocityOld.x;
    jacobian(6, 3) = delta*force * (
        cos(anglesOld.phi) * cos(anglesOld.psi) * cos(anglesOld.theta) 
        );
    jacobian(7, 3) = delta*force * (
        -sin(anglesOld.phi) * cos(anglesOld.psi) * sin(anglesOld.theta)
        -sin(anglesOld.psi) * cos(anglesOld.theta)
        );
    jacobian(8, 3) = delta*force * (
        -sin(anglesOld.phi) * sin(anglesOld.psi) * cos(anglesOld.theta)
        -cos(anglesOld.psi) * sin(anglesOld.theta)
        );
    // velocity y
    jacobian(4, 4) = 1 - parameters_[2] * delta - parameters_[3] * 2 * delta * velocityOld.y;
    jacobian(6, 4) = delta*force * (
        -cos(anglesOld.phi) * sin(anglesOld.psi) * cos(anglesOld.theta)
        );
    jacobian(7, 4) = delta*force * (
        sin(anglesOld.phi) * sin(anglesOld.psi) * sin(anglesOld.theta)
        - cos(anglesOld.psi) * cos(anglesOld.theta)
        );
    jacobian(8, 4) = delta*force * (
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
    jacobian(9, 8) = delta;
    // rotation speed psi
    jacobian(9, 9) = 1 - delta*parameters_[7];

    // normal noise
    mat noiseTransf(4, 10, fill::zeros);
    // gaz
    noiseTransf(3, 5) = delta * parameters_[8];
    // phi
    noiseTransf(0, 6) = delta * parameters_[4];
    // theta
    noiseTransf(1, 7) = delta * parameters_[4];
    // rotation speed psi
    noiseTransf(2, 9) = delta * parameters_[6];

    // ======= predict state deviation ===========

    mat noise(4, 4, fill::zeros);
    noise(0, 0) = normalDistribution_(randomGenerator_) * flyControl.roll * flyControl.roll;
    noise(1, 1) = normalDistribution_(randomGenerator_) * flyControl.pitch * flyControl.pitch;
    noise(2, 2) = normalDistribution_(randomGenerator_) * flyControl.yaw * flyControl.yaw;
    noise(3, 3) = normalDistribution_(randomGenerator_) * flyControl.gaz * flyControl.gaz;

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

    // additional noise 
    mat noise(7, 7, fill::zeros);
    noise(0, 0) = normalDistribution_(randomGenerator_) * imuMeasurement.altitude * imuMeasurement.altitude;
    noise(1, 1) = normalDistribution_(randomGenerator_) * imuMeasurement.velocity.x * imuMeasurement.velocity.x;
    noise(2, 2) = normalDistribution_(randomGenerator_) * imuMeasurement.velocity.y * imuMeasurement.velocity.y;
    noise(3, 3) = normalDistribution_(randomGenerator_) * imuMeasurement.velocity.z * imuMeasurement.velocity.z;
    noise(4, 4) = normalDistribution_(randomGenerator_) * imuMeasurement.angles.phi * imuMeasurement.angles.phi;
    noise(5, 5) = normalDistribution_(randomGenerator_) * imuMeasurement.angles.theta * imuMeasurement.angles.theta;
    noise(6, 6) = normalDistribution_(randomGenerator_) * imuMeasurement.angularRotationPsi * imuMeasurement.angularRotationPsi;

    // compute kalman gain
    mat gain = state.second * measurementJacobian.t() * (measurementJacobian * state.second * measurementJacobian.t() + noise).i();

    // compute predicted measurement 
    mat predictedMeasurment(7, 1, fill::zeros);
    predictedMeasurment(0, 0) = state.first.position.z;
    predictedMeasurment(1, 0) = state.first.velocity.x * cos(state.first.angles.psi) 
        - state.first.velocity.y * sin(state.first.angles.psi);
    predictedMeasurment(2, 0) = state.first.velocity.x * sin(state.first.angles.psi)
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
randomGenerator_(5),
startTime_(Clock::now()){

}

void Ekf::flyControl(const FlyControl &flyControl, const long int &timestamp){
    flyControls_.push(FlyControlChronologic(flyControl, timestamp));
};

void Ekf::measurement(const DroneStateMeasurement &measurement, const long int &timestamp){
    measurements_.push(MeasurementChronologic(measurement, timestamp));
};
