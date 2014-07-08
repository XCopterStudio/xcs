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

Predictor::Predictor() {
    resetPos();
    lastAddedDronetime = 0;
}

Predictor::~Predictor(void) {
}


// watch out: does NOT update any matrices, only (x,y,z,r,p,y)!!!!!!!
// also: does not filter z-data, only sets corrupted-flag...

void Predictor::predictOneStep(const ImuMeasurementChronologic &imuMeasurement) {
    // angles
    roll = imuMeasurement.first.angles.phi;
    pitch = imuMeasurement.first.angles.theta;
    yaw = imuMeasurement.first.angles.psi;

    double timespan = imuMeasurement.second - lastAddedDronetime; // in seconds
    lastAddedDronetime = imuMeasurement.second;
    if (timespan > 0.05 || timespan < 1e-6) {
        timespan = std::max(0.0, std::min(0.005, timespan)); // clamp strange values
    }

    // horizontal speed integration
    // (mm / s)/1.000 * (mics/1.000.000) = meters.
    double dxDrone = imuMeasurement.first.velocity.x * timespan; // in meters
    double dyDrone = imuMeasurement.first.velocity.y * timespan; // in meters

    yaw = imuMeasurement.first.angles.psi;
    x += cos(yaw) * dxDrone + sin(yaw) * dyDrone;
    y += -sin(yaw) * dxDrone + cos(yaw) * dyDrone;

    // height
    if (abs(z - imuMeasurement.first.altitude) > 0.12) {
        if (std::abs(z - imuMeasurement.first.altitude) > abs(zCorruptedJump)) {
            zCorruptedJump = z - imuMeasurement.first.altitude;
        }
        zCorrupted = true;
    }

    z = imuMeasurement.first.altitude;


}

void Predictor::resetPos() {
    zCorrupted = false;
    zCorruptedJump = 0;
    x = y = z = 0;
    roll = pitch = yaw = 0;
}
