#include "tum_utils.hpp"

using namespace xcs::nodes::ptam;

TooN::Vector<3> TumUtils::transformPTAMObservation(double x, double y, double z) {
    //return transformPTAMObservation(x, y, z, yaw.state[0]); TODO changed
    return transformPTAMObservation(x, y, z, yaw);
}

TooN::Vector<3> TumUtils::transformPTAMObservation(double x, double y, double z, double yaw) {
    double yawRad = yaw * 3.14159268 / 180;
    x = x_offset + xy_scale * x - 0.2 * sin(yawRad);
    y = y_offset + xy_scale * y - 0.2 * cos(yawRad);
    z = z_offset + z_scale*z;
    return TooN::makeVector(x, y, z);
}

TooN::Vector<6> TumUtils::transformPTAMObservation(TooN::Vector<6> obs) {
    obs.slice<0, 3>() = transformPTAMObservation(obs[0], obs[1], obs[2], obs[5]);

    obs[3] += roll_offset;
    obs[4] += pitch_offset;
    obs[5] += yaw_offset;

    return obs;
}

TooN::Vector<6> TumUtils::backTransformPTAMObservation(TooN::Vector<6> obs) {
    obs[3] -= roll_offset;
    obs[4] -= pitch_offset;
    obs[5] -= yaw_offset;

    double yawRad = obs[5] * 3.14159268 / 180;
    obs[0] = (-x_offset + obs[0] + 0.2 * sin(yawRad)) / xy_scale;
    obs[1] = (-y_offset + obs[1] + 0.2 * cos(yawRad)) / xy_scale;
    obs[2] = (-z_offset + obs[2]) / z_scale;

    return obs;
}

