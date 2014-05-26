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

#include <xcs/types/timestamp.hpp>
#include <xcs/types/cartesian_vector.hpp>
#include <xcs/types/eulerian_vector.hpp>

using namespace xcs::nodes::localization;

const TooN::SE3<double> Predictor::droneToBottom = TooN::SE3<double>(TooN::SO3<double>(TooN::makeVector(3.14159265,0,0)),TooN::makeVector(0,0,0));
const TooN::SE3<double> Predictor::bottomToDrone = Predictor::droneToBottom.inverse();

const TooN::SE3<double> Predictor::droneToFront = TooN::SE3<double>(TooN::SO3<double>(TooN::makeVector(3.14159265/2,0,0)),TooN::makeVector(0,0.025,-0.2));
const TooN::SE3<double> Predictor::frontToDrone = Predictor::droneToFront.inverse();

const TooN::SE3<double> Predictor::droneToFrontNT = TooN::SE3<double>(TooN::SO3<double>(TooN::makeVector(3.14159265/2,0,0)),TooN::makeVector(0,0,0));
const TooN::SE3<double> Predictor::frontToDroneNT = Predictor::droneToFrontNT.inverse();


// load distortion matrices
Predictor::Predictor(std::string basePath)
{

	setPosRPY(0,0,0,0,0,0);
	lastAddedDronetime = 0;
}


Predictor::~Predictor(void)
{
}

 void Predictor::calcCombinedTransformations()
{
	globalToFront = droneToFront * globaltoDrone;
	globalToBottom = droneToBottom * globaltoDrone;
	frontToGlobal = globalToFront.inverse();
	bottmoToGlobal = globalToBottom.inverse();
}


// input in rpy
void Predictor::setPosRPY(double newX, double newY, double newZ, double newRoll, double newPitch, double newYaw)
{
	// set rpy
	x = newX; y = newY; z = newZ;
	roll = newRoll; pitch = newPitch; yaw = newYaw;

	// set se3
	droneToGlobal.get_translation()[0] = x;
	droneToGlobal.get_translation()[1] = y;
	droneToGlobal.get_translation()[2] = z;
	droneToGlobal.get_rotation() = rpy2rod(roll,pitch,yaw);

	globaltoDrone = droneToGlobal.inverse();

	// set rest
	calcCombinedTransformations();
}

// input in SE3
void Predictor::setPosSE3_globalToDrone(TooN::SE3<double> newGlobaltoDrone)
{
	// set se3
	globaltoDrone = newGlobaltoDrone;
	droneToGlobal = globaltoDrone.inverse();

	// set rpy
	x = droneToGlobal.get_translation()[0];
	y = droneToGlobal.get_translation()[1];
	z = droneToGlobal.get_translation()[2];
	rod2rpy(droneToGlobal.get_rotation(),&roll,&pitch,&yaw);

	// set rest
	calcCombinedTransformations();
}
void Predictor::setPosSE3_droneToGlobal(TooN::SE3<double> newDroneToGlobal)
{
	droneToGlobal = newDroneToGlobal;
	globaltoDrone = droneToGlobal.inverse();
	
	x = droneToGlobal.get_translation()[0];
	y = droneToGlobal.get_translation()[1];
	z = droneToGlobal.get_translation()[2];

	rod2rpy(droneToGlobal.get_rotation(),&roll,&pitch,&yaw);

	calcCombinedTransformations();
}


// watch out: does NOT update any matrices, only (x,y,z,r,p,y)!!!!!!!
// also: does not filter z-data, only sets corrupted-flag...

// TODO!! Convert units
void Predictor::predictOneStep(const xcs::Timestamp timestamp, const double altitude, const xcs::CartesianVector velocity, const xcs::EulerianVector rotation)
{
	double timespan = timestamp - lastAddedDronetime;	// in micros
	lastAddedDronetime = timestamp;
	if(timespan > 50000 || timespan < 1)
		timespan = std::max(0.0,std::min(5000.0,timespan));	// clamp strange values

	// horizontal speed integration
	// (mm / s)/1.000 * (mics/1.000.000) = meters.
	double dxDrone = velocity.x * timespan / 1000000000;	// in meters
	double dyDrone = velocity.y * timespan / 1000000000;	// in meters

	double yawRad = (rotation.psi/1000.0) / (180.0/3.1415); // TODO units
	x += sin(yawRad)*dxDrone+cos(yawRad)*dyDrone;
	y += cos(yawRad)*dxDrone-sin(yawRad)*dyDrone;

	// height
	if(abs(z - altitude*0.001) > 0.12)
	{
		if(std::abs(z - altitude*0.001) > abs(zCorruptedJump))
			zCorruptedJump = z - altitude*0.001;
		zCorrupted = true;
	}

	z = altitude*0.001;

	// angles
	roll = rotation.phi;
	pitch = rotation.theta;
	yaw = rotation.psi;

}
void Predictor::resetPos()
{
	zCorrupted = false;
	zCorruptedJump = 0;
	setPosRPY(0,0,0,0,0,0);
}
