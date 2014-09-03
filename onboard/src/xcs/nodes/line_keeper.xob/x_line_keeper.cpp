#include "x_line_keeper.hpp"
#include "xcs/nodes/line_drawer.xob/x_line_drawer.hpp"
#include <chrono>
#include <cmath>

using namespace xcs;
using namespace xcs::nodes;
using namespace std;
using namespace std::chrono;

const size_t XLineKeeper::REFRESH_PERIOD = 20; // ms

//TODO remove static_cast<EulerianVector> (rotation.data()).psi in favor of static_cast<EulerianVector> (rotation).psi
XLineKeeper::XLineKeeper(const string& name) :
  XObject(name),
  velocity("VELOCITY_LOC"),
  altitude("ALTITUDE"),
  rotation("ROTATION"),
  distance("DISTANCE"),
  deviation("DEVIATION"),
  isKeeping_(false) {
    XBindFunction(XLineKeeper, init);
    XBindFunction(XLineKeeper, setLineDrawer);
    XBindFunction(XLineKeeper, reset);
    /*XBindFunction(XLineKeeper, stop);*/

    XBindVarF(velocity, &XLineKeeper::onChangeVelocity);

    XBindVarF(altitude, &XLineKeeper::onChangeAltitude);
    XBindVarF(rotation, &XLineKeeper::onChangeRotation);

    UBindVar(XLineKeeper, cameraScale);

    XBindVar(distance);
    XBindVar(deviation);
}

void XLineKeeper::init() {
    USetUpdate(REFRESH_PERIOD);
}

int XLineKeeper::update() {
    if (!isKeeping_) {
        return 0;
    }
    /*
     * Calculate distance
     */
    VectorType deviationVector(cos(initialDeviation_), sin(initialDeviation_));
    auto scale = static_cast<double> (cameraScale) / (altitude_ + 1e-9);
    VectorType scaledPositionShift;
    /*
     * drone coordinates: x (forward), y (leftward)
     * image coordinates: x (rightward), y (upward) //TODO check
     */
    scaledPositionShift.x = scale * -positionShift_.y;
    scaledPositionShift.y = scale * positionShift_.x;

    /* D = D_l + d * r (scalar product) */
    distance = initialDistance_ + (deviationVector.x * scaledPositionShift.x + deviationVector.y * scaledPositionShift.y);

    /*
     * Calculate deviation
     */
    deviation = -rotation_.psi + initialDevOffset_; // beware: psi has opposite sign

    /*
     * Debug draw
     */
    if (lineDrawer_ != nullptr){
        lineDrawer_->drawFullLine(distance.data(), deviation.data(), cv::Scalar(0, 255, 255), 3);
    }

    return 0; // Urbi undocumented, return value probably doesn't matter
}

void XLineKeeper::setLineDrawer(UObject *drawer) {
    //lineDrawer_ = dynamic_cast<XLineDrawer *>(drawer);
    lineDrawer_ = (XLineDrawer *) (drawer); //TODO missing typeinfo when linking
}

void XLineKeeper::reset(double distance, double deviation) {
    initialDistance_ = distance;
    initialDeviation_ = deviation;
    initialDevOffset_ = deviation + rotation_.psi; // beware: psi has opposite sign, this is difference

    positionShift_.x = 0;
    positionShift_.y = 0;

    lastTimeVx_ = lastTimeVy_ = high_resolution_clock::now();
    isKeeping_ = true;
}

//void XLineKeeper::stop() {
//    isKeeping_ = false;
//}

void XLineKeeper::onChangeVelocity(const xcs::CartesianVector v) {
    auto now = high_resolution_clock::now();
    // TODO refactor nicer
    auto elapsedX = duration<double, seconds::period>(now - lastTimeVx_).count();
    auto elapsedY = duration<double, seconds::period>(now - lastTimeVy_).count();
    positionShift_.x += v.x * elapsedX;
    positionShift_.y += v.y * elapsedY;
    lastTimeVy_ = lastTimeVx_ = now;
}


UStart(XLineKeeper);
