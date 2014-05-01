#include "hermit_movement.hpp"

#include <iostream>
#include <armadillo>
#include <cmath>

#include <xcs/xcs_fce.hpp>

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

// ================== public function ======================

HermitMovement::HermitMovement()
{
    newCheckpoint_ = true;
    clear_ = false;
}

void HermitMovement::dronePosition(const xcs::CartesianVector &dronePosition){
    dronePosition_ = dronePosition;
}

void HermitMovement::droneVelocity(const xcs::CartesianVector &droneVelocity){
    droneVelocity_ = droneVelocity;
}

void HermitMovement::droneRotation(const xcs::EulerianVector &droneRotation){
    droneRotation_ = droneRotation;
}

xcs::SpeedControl HermitMovement::flyOnCheckpoint(const double &speed){
    if (clear_){
        return SpeedControl();
        clear_ = false;
    }

    if (newCheckpoint_){
        empty_ = !(checkpointQueue_.tryPop(targetCheckpoint));
    }

    if (!empty_){
        double distance = computeDistance(targetCheckpoint, dronePosition_);
        double step = 1.0 / (distance * POINTS_ON_METER);
        double boundSpeed = valueInRange(0.0, MAX_SPEED, speed);

        Checkpoint droneCheckpoint(dronePosition_.x, dronePosition_.y, dronePosition_.z,
            droneVelocity_.x, droneVelocity_.y, droneVelocity_.z);

        Checkpoint interCheckpoint = computeHermitPoint(droneCheckpoint, targetCheckpoint, step);

        double deltaX = interCheckpoint.x - dronePosition_.x;
        double deltaY = interCheckpoint.y - dronePosition_.y;
        double deltaZ = interCheckpoint.z - dronePosition_.z;

        double norm = boundSpeed / std::max(deltaX, std::max(deltaY, deltaZ));

        if (distance < EPSILON){
            newCheckpoint_ = true;
        }

        //TODO: use yaw difference
        return SpeedControl(norm*deltaX, norm*deltaY, norm*deltaZ, 0);
    }
    else{
        return SpeedControl();
    }
}

void HermitMovement::addCheckpoint(const Checkpoint &checkpoint){
    checkpointQueue_.push(checkpoint);
}

void HermitMovement::deleteCheckpoints(){
    checkpointQueue_.popAll();
    clear_ = true;
}