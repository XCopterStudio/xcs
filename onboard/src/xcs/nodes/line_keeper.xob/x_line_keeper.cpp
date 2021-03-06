#include "x_line_keeper.hpp"
#include "xcs/nodes/line_drawer.xob/x_line_drawer.hpp"
#include <chrono>
#include <cmath>

using namespace xcs;
using namespace xcs::nodes;
using namespace std;
using namespace std::chrono;


//TODO remove static_cast<EulerianVector> (rotation.data()).psi in favor of static_cast<EulerianVector> (rotation).psi

XLineKeeper::XLineKeeper(const string& name) :
  XObject(name),
  velocity("VELOCITY_LOC"),
  altitude("ALTITUDE"),
  rotation("ROTATION"),
  initialDistance("DISTANCE"),
  initialDeviation("DEVIATION"),
  distance("DISTANCE"),
  deviation("DEVIATION"),
  isKeeping_(false) {
    XBindFunction(XLineKeeper, init);


    XBindVarF(velocity, &XLineKeeper::onChangeVelocity);
    XBindVarF(altitude, &XLineKeeper::onChangeAltitude);
    XBindVarF(rotation, &XLineKeeper::onChangeRotation);

    XBindVarF(initialDistance, &XLineKeeper::onChangeInitialDistance);
    XBindVarF(initialDeviation, &XLineKeeper::onChangeInitialDeviation);

    UBindVar(XLineKeeper, cameraScale);

    XBindVar(distance);
    XBindVar(deviation);
}

void XLineKeeper::init() {

}

void XLineKeeper::stateChanged(XObject::State state) {
    switch (state) {
        case STATE_STARTED:
            isKeeping_ = true;
            break;
        case STATE_STOPPED:
            isKeeping_ = false;
            break;
    }
}

void XLineKeeper::onChangeVelocity(const xcs::CartesianVector v) {
    auto now = high_resolution_clock::now();
    // TODO refactor nicer
    auto elapsedX = duration<double, seconds::period>(now - lastTimeVx_).count();
    auto elapsedY = duration<double, seconds::period>(now - lastTimeVy_).count();
    positionShift_.x += v.x * elapsedX;
    positionShift_.y += v.y * elapsedY;
    lastTimeVy_ = lastTimeVx_ = now;

    if (isKeeping_) {
        updateOutput();
    }
}

void XLineKeeper::updateOutput() {
    /*
     * Calculate distance
     */
    VectorType deviationVector(cos(initialDeviation_), sin(initialDeviation_));
    auto scale = static_cast<double> (cameraScale) / (altitude_ + 1e-9);
    VectorType scaledPositionShift;
    /*
     * drone coordinates: x (forward), y (leftward)
     * image coordinates: x (rightward), y (upward)
     */
    scaledPositionShift.x = scale * -positionShift_.y;
    scaledPositionShift.y = scale * positionShift_.x;

    /* D = D_l + d * r (scalar product) */
    distance = initialDistance_ + (deviationVector.x * scaledPositionShift.x + deviationVector.y * scaledPositionShift.y);

    /*
     * Calculate deviation
     */
    deviation = -rotation_.psi + initialDevOffset_; // beware: psi has opposite sign
}

void XLineKeeper::reset(const double distance, const double deviation) {
    initialDistance_ = distance;
    initialDeviation_ = deviation;
    initialDevOffset_ = deviation + rotation_.psi; // beware: psi has opposite sign, this is difference

    positionShift_.x = 0;
    positionShift_.y = 0;

    lastTimeVx_ = lastTimeVy_ = high_resolution_clock::now();
    isKeeping_ = true;
}



XStart(XLineKeeper);
