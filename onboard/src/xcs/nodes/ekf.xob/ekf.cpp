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

DroneStateDistribution Ekf::predict(const DroneStateDistribution &state, const FlyParam &flyparam, const double &delta){
    DroneStateDistribution predictedState = state;
    const CartesianVector &positionOld = state.first.position;
    const CartesianVector &velocityOld = state.first.velocity;
    const EulerianVector &anglesOld = state.first.angles;

    // predict acceleration
    // acceleration in drone frame
    double force = parameters[1] * (1.0 + parameters[0] * flyparam.gaz);
    double forceX = force * sin(anglesOld.phi)*cos(anglesOld.theta);
    double forceY = -force * sin(anglesOld.theta);
    // friction
    double frictionX = parameters[2] * velocityOld.x + parameters[3] * velocityOld.x * velocityOld.x;
    double frictionY = parameters[2] * velocityOld.y + parameters[3] * velocityOld.y * velocityOld.y;

    // drone acceleration in global frame
    CartesianVector acceleration;
    acceleration.x = (cos(anglesOld.psi)*forceX + sin(anglesOld.psi)*forceY) - frictionX;
    acceleration.y = (-sin(anglesOld.psi)*forceX + cos(anglesOld.psi)*forceY) - frictionY;
    acceleration.z = parameters[8] * (1.0 + parameters[0] * flyparam.gaz) - parameters[9] * velocityOld.z;

    // angular rotation speed
    EulerianVector angularRotation;
    angularRotation.phi = parameters[4] * flyparam.roll - parameters[5] * anglesOld.phi;
    angularRotation.theta = parameters[4] * flyparam.pitch - parameters[5] * anglesOld.theta;
    angularRotation.psi = parameters[6] * flyparam.yaw - parameters[7] * anglesOld.psi;

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
    angles.psi += anglesOld.psi*delta + angularRotation.psi*delta*delta / 2.0;
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
    jacobian[4, 4] = 1 - parameters[2] * delta - parameters[3] * 2 * delta * velocityOld.x;
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
    jacobian[5, 5] = 1 - parameters[2] * delta - parameters[3] * 2 * delta * velocityOld.y;
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
    jacobian[6, 6] = 1 - delta*parameters[9];
    // angle phi
    jacobian[7, 7] = 1 - delta*parameters[5];
    // angle theta
    jacobian[8, 8] = 1 - delta*parameters[5];
    // angle psi
    jacobian[9, 9] = 1;
    jacobian[10, 9] = delta;
    // rotation speed psi
    jacobian[10, 10] = 1 - delta*parameters[7];

    // normal noise
    mat noiseTransf(4, 10, fill::zeros);
    // gaz
    noiseTransf[4, 6] = delta * parameters[8];
    // phi
    noiseTransf[1, 7] = delta * parameters[4];
    // theta
    noiseTransf[2, 8] = delta * parameters[4];
    // rotation speed psi
    noiseTransf[3, 10] = delta * parameters[6];

    // ======= predict state deviation ===========

    mat noise(4, 4, fill::zeros);
    noise[1, 1] = normalDistribution(randomGenerator) * flyparam.roll * flyparam.roll;
    noise[2, 2] = normalDistribution(randomGenerator) * flyparam.pitch * flyparam.pitch;
    noise[3, 3] = normalDistribution(randomGenerator) * flyparam.yaw * flyparam.yaw;
    noise[4, 4] = normalDistribution(randomGenerator) * flyparam.gaz * flyparam.gaz;

    mat deviation(10, 10);
    deviation = jacobian * state.second.getMat() * jacobian.t() + noiseTransf * noise * noiseTransf.t();
    predictedState.second.Mat(deviation);

    // ======= end predict state deviation ===========

    return predictedState;
}

// =========================== public functions ============================

Ekf::Ekf() : 
randomGenerator(5){

}