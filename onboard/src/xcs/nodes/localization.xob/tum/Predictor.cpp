/**
 *  This file is part of tum_ardrone.
 *
 *  Copyright 2012 Jakob Engel <jajuengel@gmail.com> (Technical University of Munich)
 *  For more information see <https://vision.in.tum.de/data/software/tum_ardrone>.
 *
 *  tum_ardrone is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  tum_ardrone is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with tum_ardrone.  If not, see <http://www.gnu.org/licenses/>.
 */



#include "Predictor.h"
#include "tum_utils.hpp"

#include "../ekf.hpp"

using namespace xcs::nodes::localization;

const TooN::SE3<double> Predictor::droneToBottom = TooN::SE3<double>(TooN::SO3<double>(TooN::makeVector(3.14159265, 0, 0)), TooN::makeVector(0, 0, 0));
const TooN::SE3<double> Predictor::bottomToDrone = Predictor::droneToBottom.inverse();

const TooN::SE3<double> Predictor::droneToFront = TooN::SE3<double>(TooN::SO3<double>(TooN::makeVector(3.14159265 / 2, 0, 0)), TooN::makeVector(0, 0.025, -0.2));
const TooN::SE3<double> Predictor::frontToDrone = Predictor::droneToFront.inverse();

const TooN::SE3<double> Predictor::droneToFrontNT = TooN::SE3<double>(TooN::SO3<double>(TooN::makeVector(3.14159265 / 2, 0, 0)), TooN::makeVector(0, 0, 0));
const TooN::SE3<double> Predictor::frontToDroneNT = Predictor::droneToFrontNT.inverse();

Predictor::Predictor() {
    setPosRPY(0, 0, 0, 0, 0, 0);
    lastAddedDronetime = 0;
}

Predictor::~Predictor(void) {
}

void Predictor::calcCombinedTransformations() {
    globalToFront = droneToFront * globaltoDrone;
    globalToBottom = droneToBottom * globaltoDrone;
    frontToGlobal = globalToFront.inverse();
    bottmoToGlobal = globalToBottom.inverse();
}


// input in rpy

void Predictor::setPosRPY(double newX, double newY, double newZ, double newRoll, double newPitch, double newYaw) {
    // set rpy
    x = newX;
    y = newY;
    z = newZ;
    roll = newRoll;
    pitch = newPitch;
    yaw = newYaw;

    // set se3
    droneToGlobal.get_translation()[0] = x;
    droneToGlobal.get_translation()[1] = y;
    droneToGlobal.get_translation()[2] = z;
    droneToGlobal.get_rotation() = rpy2rod(roll, pitch, yaw);

    globaltoDrone = droneToGlobal.inverse();

    // set rest
    calcCombinedTransformations();
}

// input in SE3

void Predictor::setPosSE3_globalToDrone(TooN::SE3<double> newGlobaltoDrone) {
    // set se3
    globaltoDrone = newGlobaltoDrone;
    droneToGlobal = globaltoDrone.inverse();

    // set rpy
    x = droneToGlobal.get_translation()[0];
    y = droneToGlobal.get_translation()[1];
    z = droneToGlobal.get_translation()[2];
    rod2rpy(droneToGlobal.get_rotation(), &roll, &pitch, &yaw);

    // set rest
    calcCombinedTransformations();
}

void Predictor::setPosSE3_droneToGlobal(TooN::SE3<double> newDroneToGlobal) {
    droneToGlobal = newDroneToGlobal;
    globaltoDrone = droneToGlobal.inverse();

    x = droneToGlobal.get_translation()[0];
    y = droneToGlobal.get_translation()[1];
    z = droneToGlobal.get_translation()[2];

    rod2rpy(droneToGlobal.get_rotation(), &roll, &pitch, &yaw);

    calcCombinedTransformations();
}


// watch out: does NOT update any matrices, only (x,y,z,r,p,y)!!!!!!!
// also: does not filter z-data, only sets corrupted-flag...

void Predictor::predictOneStep(const ImuMeasurementChronologic &imuMeasurement) {
    double timespan = imuMeasurement.second - lastAddedDronetime; // in seconds
    lastAddedDronetime = imuMeasurement.second;
    if (timespan > 0.05 || timespan < 1e-6)
        timespan = std::max(0.0, std::min(0.005, timespan)); // clamp strange values

    // horizontal speed integration
    // (mm / s)/1.000 * (mics/1.000.000) = meters.
    double dxDrone = imuMeasurement.first.velocity.x * timespan; // in meters
    double dyDrone = imuMeasurement.first.velocity.y * timespan; // in meters

    double yawRad = imuMeasurement.first.angles.psi;
    x += sin(yawRad) * dxDrone + cos(yawRad) * dyDrone;
    y += cos(yawRad) * dxDrone - sin(yawRad) * dyDrone;

    // height
    if (abs(z - imuMeasurement.first.altitude) > 0.12) {
        if (std::abs(z - imuMeasurement.first.altitude) > abs(zCorruptedJump))
            zCorruptedJump = z - imuMeasurement.first.altitude;
        zCorrupted = true;
    }

    z = imuMeasurement.first.altitude;

    // angles
    roll = imuMeasurement.first.angles.phi;
    pitch = imuMeasurement.first.angles.theta;
    yaw = imuMeasurement.first.angles.psi;
}

void Predictor::resetPos() {
    zCorrupted = false;
    zCorruptedJump = 0;
    setPosRPY(0, 0, 0, 0, 0, 0);
}
