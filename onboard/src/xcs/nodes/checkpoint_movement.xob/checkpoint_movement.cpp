#include "checkpoint_movement.hpp"

#include <iostream>
#include <armadillo>
#include <cmath>

#include <xcs/xcs_fce.hpp>

using namespace arma;
using namespace xcs;
using namespace xcs::nodes::hermit;

const unsigned int CheckpointMovement::POINTS_ON_METER = 100;
const double CheckpointMovement::EPSILON = 0.1;
const double CheckpointMovement::MAX_SPEED = 0.5;

double CheckpointMovement::computeDistance(const xcs::Checkpoint &targetCheckpoint, const xcs::CartesianVector &actualPosition){
    double temp = hypot(targetCheckpoint.x - actualPosition.x, targetCheckpoint.y - actualPosition.y);
    return hypot(temp,targetCheckpoint.z - actualPosition.z);
}

xcs::Checkpoint CheckpointMovement::computeHermitPoint(const xcs::Checkpoint &start, const xcs::Checkpoint &end, double step){
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

CheckpointMovement::CheckpointMovement(std::function<void(bool)> reachedCallback) : reachedCallback_(std::move(reachedCallback))
{
    newCheckpoint_ = true;
    clear_ = false;
}

void CheckpointMovement::dronePosition(const xcs::CartesianVector &dronePosition){
    dronePosition_ = dronePosition;
}

void CheckpointMovement::droneVelocity(const xcs::CartesianVector &droneVelocity){
    droneVelocity_ = droneVelocity;
}

void CheckpointMovement::droneRotation(const xcs::EulerianVector &droneRotation){
    droneRotation_ = droneRotation;
}

xcs::SpeedControl CheckpointMovement::flyOnCheckpoint(const double &speed){
    if (clear_){
        newCheckpoint_ = true;
        clear_ = false;
        return SpeedControl();
    }

    if (newCheckpoint_){
        empty_ = !(checkpointQueue_.tryPop(targetCheckpoint_));
        newCheckpoint_ = empty_;
    }

    if (!newCheckpoint_){
        if (reachedCallback_ != nullptr){
            reachedCallback_(false);
        }

        double distance = computeDistance(targetCheckpoint_, dronePosition_);
        double step = 1.0 / (distance * POINTS_ON_METER);
        double boundSpeed = valueInRange(speed, 0.0, MAX_SPEED);

        //Checkpoint droneCheckpoint(dronePosition_.x, dronePosition_.y, dronePosition_.z,
        //    valueInRange(droneVelocity_.x*distance,droneVelocity_.x), 
        //    valueInRange(droneVelocity_.y*distance, droneVelocity_.y), 
        //    valueInRange(droneVelocity_.z*distance, droneVelocity_.z));

        //Checkpoint interCheckpoint;
        //double step_temp = step;

        //do { // find point distant EPSILON from drone actual position
        //    interCheckpoint = computeHermitPoint(droneCheckpoint, targetCheckpoint_, step_temp);
        //    step_temp += step;
        //} while (distance > EPSILON && computeDistance(interCheckpoint, dronePosition_) < EPSILON);

        //double deltaX = interCheckpoint.x - dronePosition_.x;
        //double deltaY = interCheckpoint.y - dronePosition_.y;
        //double deltaZ = interCheckpoint.z - dronePosition_.z;

        double deltaX = targetCheckpoint_.x - dronePosition_.x;
        double deltaY = targetCheckpoint_.y - dronePosition_.y;
        double deltaZ = targetCheckpoint_.z - dronePosition_.z;        

        if (distance < EPSILON){
            printf("Drone achieved destination: [%f,%f,%f] \n", targetCheckpoint_.x, targetCheckpoint_.y, targetCheckpoint_.z);
            printf("Hermit: New checkpoint \n");
            newCheckpoint_ = true;
            if (reachedCallback_ != nullptr){
                reachedCallback_(true);
            }
        }
        else{
            //double norm = boundSpeed / std::max(std::abs(deltaX), std::max(std::abs(deltaY), std::abs(deltaZ)));
            //return SpeedControl(norm*deltaX, norm*deltaY, norm*deltaZ, 0);

            double norm = 4;
            return SpeedControl(valueInRange(norm*deltaX*boundSpeed, -boundSpeed, boundSpeed),
            valueInRange(norm*deltaY*boundSpeed, -boundSpeed, boundSpeed),
            valueInRange(norm*deltaZ*boundSpeed, -boundSpeed, boundSpeed),
            0
            );
        }

        printf("Drone state: [%f,%f,%f,%f] \n", dronePosition_.x, dronePosition_.y, dronePosition_.z,droneRotation_.psi);
        //TODO: use yaw difference
        //printf("Hermit: Control speed [%f,%f,%f,%f] \n", norm*deltaX*boundSpeed, norm*deltaY*boundSpeed, norm*deltaZ*boundSpeed, 0);
        
        
    }
    
    return SpeedControl();
}

void CheckpointMovement::addCheckpoint(const Checkpoint &checkpoint){
    printf("Hermit: insert checkpoint \n");
    checkpointQueue_.push(checkpoint);
}

void CheckpointMovement::deleteCheckpoints(){
    checkpointQueue_.popAll();
    clear_ = true;
}
