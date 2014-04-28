#include "hermit_movement.hpp"

#include <iostream>
#include <armadillo>

using namespace arma;
using namespace xcs;
using namespace xcs::nodes::hermit;

const unsigned int HermitMovement::pointsOnMeter = 100;

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
   
void HermitMovement::flyOnCheckpoints(){
    Checkpoint last;
    while (!endAll_){
        if(checkpointQueue_.tryPop(targetCheckpoint_) && !clear_){

            /*for (double i = 0; i <= 1; i = i + 0.1){
                Checkpoint point = computeHermitPoint(last, targetCheckpoint_, i);
                std::cerr << point.x << ", " << point.y << ", " << point.z << " ;" << std::endl;
            }
            last = targetCheckpoint_;*/
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