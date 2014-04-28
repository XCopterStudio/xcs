#include "hermit_movement.hpp"

#include <iostream>
#include <armadillo>
#include <cmath>

using namespace arma;
using namespace xcs;
using namespace xcs::nodes::hermit;

const unsigned int HermitMovement::POINTS_ON_METER = 100;
const double HermitMovement::EPSILON = 0.1;
const double HermitMovement::MAX_SPEED = 1;

double HermitMovement::computeDistance(const xcs::Checkpoint &targetCheckpoint, const xcs::CartesianVector &actualPosition){
    double temp = _hypot(targetCheckpoint.x - actualPosition.x, targetCheckpoint.y - actualPosition.y);
    return _hypot(temp,targetCheckpoint.z - actualPosition.z);
}

xcs::Checkpoint HermitMovement::computeHermitPoint(const xcs::Checkpoint &start, const xcs::Checkpoint &end, double step){
    mat steps;
    steps << step*step*step << step*step << step << 1 << endr;

    mat derSteps;
    derSteps << 3*step*step << 2*step << 1 << 0 << endr;

    mat hermitPolynomials;
    hermitPolynomials <<  2.0 << -2.0 <<  1.0 <<  1.0 << endr
                      << -3.0 << 3.0 << -2.0 << -1.0 << endr
                      << 0.0 << 0.0 << 1.0 << 0.0 << endr
                      << 1.0 << 0.0 << 0.0 << 0.0 << endr;

    mat points;
    points << start.x << start.y << start.z << endr
           << end.x << end.y << end.z << endr
           << start.xOut << start.yOut << start.zOut << endr
           << end.xOut << end.yOut << end.zOut << endr;

    mat interPoint = steps*hermitPolynomials*points;
    mat tangent = derSteps*hermitPolynomials*points;

    return Checkpoint(interPoint.at(0,0),interPoint.at(0,1),interPoint.at(0,2),
        tangent.at(0,0),tangent.at(0,1),tangent.at(0,2));
}
   
void HermitMovement::flyOnCheckpoint(const xcs::Checkpoint &targetCheckpoint){
    do{
        mtxPosition_.lock();
        CartesianVector actualPosition = dronePosition_;
        mtxPosition_.unlock();

        mtxVelocity_.lock();
        CartesianVector actualVelocity = droneVelocity_;
        mtxVelocity_.unlock();

        double psi = droneRotation_.psi;

        double distance = computeDistance(targetCheckpoint,actualPosition);
        double step = 1.0 / (distance * POINTS_ON_METER);

        Checkpoint actualCheckpoint(actualPosition.x, actualPosition.y, actualPosition.z,
            actualVelocity.x, actualVelocity.y, actualVelocity.z);

        Checkpoint interCheckpoint = computeHermitPoint(actualCheckpoint,targetCheckpoint,step);
        if (distance > EPSILON){
            double deltaX = interCheckpoint.x - actualPosition.x;
            double deltaY = interCheckpoint.y - actualPosition.y;
            double deltaZ = interCheckpoint.z - actualPosition.z;

            double max = std::max(deltaX, std::max(deltaY, deltaZ));
            // TODO: set drone speed (speed(MAX_SPEED*deltaX/max, ...))
        }
        else{
            break;
        }
    } while (true);
}

void HermitMovement::flyOnCheckpoints(){
    Checkpoint targetCheckpoint;
    while (!endAll_){
        if(checkpointQueue_.tryPop(targetCheckpoint) && !clear_){
            flyOnCheckpoint(targetCheckpoint);
        }
        else{ // None checkpoint in queue sleep while and try again.
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }

        clear_ = false;
    }
}

// ================== public function ======================

HermitMovement::HermitMovement()
{
    endAll_ = false;
    clear_ = false;

    flyOnCheckpointsThread = std::thread(&xcs::nodes::hermit::HermitMovement::flyOnCheckpoints, this);
}

void HermitMovement::dronePosition(const xcs::CartesianVector &dronePosition){
    std::lock_guard<std::mutex> lckPosition(mtxPosition_);
    dronePosition_ = dronePosition;
}

void HermitMovement::droneVelocity(const xcs::CartesianVector &droneVelocity){
    std::lock_guard<std::mutex> lckVelocity(mtxVelocity_);
    droneVelocity_ = droneVelocity;
}

void HermitMovement::droneRotation(const xcs::EulerianVector &droneRotation){
    std::lock_guard<std::mutex> lckRotation(mtxRotation_);
    droneRotation_ = droneRotation;
}

void HermitMovement::addCheckpoint(const Checkpoint &checkpoint){
    checkpointQueue_.push(checkpoint);
}

void HermitMovement::deleteCheckpoints(){
    checkpointQueue_.popAll();
    clear_ = true;
}