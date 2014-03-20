#include "u_line_keeper.hpp"
#include <chrono>
#include <cmath>

using namespace xcs::urbi;
using namespace std;
using namespace std::chrono;

const size_t ULineKeeper::REFRESH_PERIOD = 100; // ms

ULineKeeper::ULineKeeper(const string& name) :
  ::urbi::UObject(name) {
    UBindFunction(ULineKeeper, init);

    UBindVar(ULineKeeper, vx);
    UNotifyChange(vx, &ULineKeeper::onChangeVx);

    UBindVar(ULineKeeper, vy);
    UNotifyChange(vy, &ULineKeeper::onChangeVy);

    UBindVar(ULineKeeper, altitude);
    UBindVar(ULineKeeper, psi);

    UBindVar(ULineKeeper, cameraScale);

    UBindVarRename(ULineKeeper, distanceUVar, "distance");
    UBindVarRename(ULineKeeper, deviationUVar, "deviation");
}

void ULineKeeper::init() {
    /*
     * Output vars
     */
    distanceUVar = 0;
    deviationUVar = 0;

    USetUpdate(REFRESH_PERIOD);
}

int ULineKeeper::update() {
    /*
     * Calculate distance
     */
    auto distance = 0;
    VectorType deviationVector(cos(initialDeviation_), sin(initialDeviation_));
    auto scale = static_cast<double> (cameraScale) / static_cast<double> (altitude);
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
    auto deviation = -static_cast<double> (psi) + initialDevOffset_; // beware: psi has opposite sign

    /*
     * Notify
     */
    distanceUVar = distance;
    deviationUVar = deviation;

    return 0; // Urbi undocumented, return value probably doesn't matter
}

void ULineKeeper::start(double distance, double deviation) {
    initialDistance_ = distance;
    initialDeviation_ = deviation;
    initialDevOffset_ = deviation + static_cast<double> (psi); // beware: psi has opposite sign, this is difference

    positionShift_.x = 0;
    positionShift_.y = 0;

    lastTimeVx_ = lastTimeVy_ = high_resolution_clock::now();
}

void ULineKeeper::onChangeVx(double vx) {
    auto now = high_resolution_clock::now();    
    auto elapsed = duration<double, seconds::period>(now - lastTimeVx_).count();
    positionShift_.x += vx * elapsed;
    lastTimeVx_ = now;
}

void ULineKeeper::onChangeVy(double vy) {
    auto now = high_resolution_clock::now();
    auto elapsed = duration<double, seconds::period>(now - lastTimeVy_).count();
    positionShift_.y += vy * elapsed;
    lastTimeVy_ = now;
}


UStart(ULineKeeper);
