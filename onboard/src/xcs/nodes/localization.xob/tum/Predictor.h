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
#ifndef __PREDICTOR_H
#define __PREDICTOR_H




#include <TooN/TooN.h>
#include <TooN/so3.h>
#include <TooN/se3.h>
#include <string>

#include <xcs/types/timestamp.hpp>

/*!
 * Forward declaration because we need to avoid including libport/pthread.h here.
 */
namespace xcs {
namespace nodes {
namespace localization {
struct DroneStateMeasurement;
typedef std::pair<DroneStateMeasurement, double> ImuMeasurementChronologic;
}
}
}


class Predictor { // TODO wrap class in xcs::nodes::localization
public:

    // xyz-position is center of drone CS in global coordinates.
    // rpy-rotation is rpy of drone.
    double roll;
    double pitch;
    double yaw;
    double x;
    double y;
    double z;
    bool zCorrupted;
    xcs::Timestamp lastAddedDronetime;
    double zCorruptedJump;

    // -------------------------- prediction -----------------------------------------------------------------------

    void predictOneStep(const xcs::nodes::localization::ImuMeasurementChronologic &imuMeasurement);
    void resetPos();

    Predictor();
    ~Predictor(void);
};
#endif /* __PREDICTOR_H */

