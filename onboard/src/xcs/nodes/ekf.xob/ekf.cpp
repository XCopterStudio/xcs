#include "ekf.hpp"

#include <cmath>

using namespace std;
using namespace xcs::nodes::ekf;
using namespace xcs::nodes::xci;

DroneStateDistribution Ekf::predict(const DroneStateDistribution &state, const FlyParam &flyparam, const double &delta){
    DroneStateDistribution predictedState = state;

    // predict acceleration
    // acceleration in drone frame
    double force = parameters[0] * (1.0 + flyparam.gaz);
    double forceX = force * sin(state.first.angles.phi)*cos(state.first.angles.theta);
    double forceY = force * cos(state.first.angles.phi)*sin(state.first.angles.theta);
    // friction
    double frictionX = parameters[1] * state.first.velocity.x + parameters[2] * state.first.velocity.x * state.first.velocity.x;
    double frictionY = parameters[1] * state.first.velocity.y + parameters[2] * state.first.velocity.y * state.first.velocity.y;

    // drone acceleration in global frame
    CartesianVector acceleration;
    acceleration.x = ( cos(state.first.angles.psi)*forceX - sin(state.first.angles.psi)*forceY) - frictionX;
    acceleration.y = (-sin(state.first.angles.psi)*forceX - cos(state.first.angles.psi)*forceY) - frictionY;
    acceleration.z = parameters[7] * (1.0 + flyparam.gaz) - parameters[8] * state.first.velocity.z;
    // angular rotation speed
    EulerianVector angularRotation;
    const EulerianVector &anglesOld = state.first.angles;
    angularRotation.phi = parameters[3] * flyparam.roll - parameters[4] * anglesOld.phi;
    angularRotation.theta = parameters[3] * flyparam.pitch - parameters[4] * anglesOld.theta;
    angularRotation.psi = parameters[5] * flyparam.yaw - parameters[6] * anglesOld.psi;

    // =========== predict new state ============
    // position
    CartesianVector &position = predictedState.first.position;
    const CartesianVector &velocityOld = state.first.velocity;
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

    // predict deviation

    return predictedState;
}

// =========================== public functions ============================