#include "scale_estimation.hpp"

#include <xcs/logging.hpp>
#include <algorithm>

using namespace xcs::nodes::localization;

TooN::Vector<6> ScaleEstimation::getCurrentOffsets() {
    TooN::Vector<6> res = TooN::makeVector(0, 0, 0, 0, 0, 0);
    if (offsets_xyz_initialized)
        res.slice<0, 3>() = TooN::makeVector(x_offset, y_offset, z_offset);
    if (rp_offset_framesContributed > 1)
        res.slice<3, 3>() = TooN::makeVector(roll_offset, pitch_offset, yaw_offset);
    return res;
}

TooN::Vector<3> ScaleEstimation::getCurrentScales() {
    return TooN::makeVector(scale_xyz_initialized ? xy_scale : 1, scale_xyz_initialized ? xy_scale : 1, scale_xyz_initialized ? z_scale : 1);
}

void ScaleEstimation::setCurrentScales(TooN::Vector<3> scales) {
    if (allSyncLocked) return;
    xy_scale = scales[0];
    z_scale = scales[0];
    scale_from_xy = scale_from_z = scales[0];

    xyz_sum_IMUxIMU = 0.2 * scales[0];
    xyz_sum_PTAMxPTAM = 0.2 / scales[0];
    xyz_sum_PTAMxIMU = 0.2;

    scalePairs_.clear();

    scalePairs_.push_back(ScaleStruct(
            TooN::makeVector(0.2, 0.2, 0.2) / sqrt(scales[0]),
            TooN::makeVector(0.2, 0.2, 0.2) * sqrt(scales[0])
            ));



    scale_xyz_initialized = true;
    offsets_xyz_initialized = false;

    initialScaleSet = scales[0];
}

void ScaleEstimation::updateScaleXYZ(TooN::Vector<3> ptamDiff, TooN::Vector<3> imuDiff, TooN::Vector<3> OrgPtamPose) {
    if (allSyncLocked) return;

    ScaleStruct s = ScaleStruct(ptamDiff, imuDiff);

    // dont add samples that are way to small...
    if (s.imuNorm < 0.05 || s.ptamNorm < 0.05) return;


    // update running sums
    scalePairs_.push_back(s);

    double xyz_scale_old = xy_scale;


    // find median.
    std::sort(scalePairs_.begin(), scalePairs_.end());
    double median = scalePairs_[(scalePairs_.size() + 1) / 2].alphaSingleEstimate;

    // hack: if we have only few samples, median is unreliable (maybe 2 out of 3 are completely wrong.
    // so take first scale pair in this case (i.e. the initial scale)
    if (scalePairs_.size() < 5)
        median = initialScaleSet;

    // find sums and median.
    // do separately for xy and z and xyz-all and xyz-filtered
    double sumII = 0;
    double sumPP = 0;
    double sumPI = 0;
    double totSumII = 0;
    double totSumPP = 0;
    double totSumPI = 0;

    double sumIIxy = 0;
    double sumPPxy = 0;
    double sumPIxy = 0;
    double sumIIz = 0;
    double sumPPz = 0;
    double sumPIz = 0;

    int numIn = 0;
    int numOut = 0;
    for (unsigned int i = 0; i < scalePairs_.size(); i++) {
        if (scalePairs_.size() < 5 || (scalePairs_[i].alphaSingleEstimate > median * 0.2 && scalePairs_[i].alphaSingleEstimate < median / 0.2)) {
            sumII += scalePairs_[i].ii;
            sumPP += scalePairs_[i].pp;
            sumPI += scalePairs_[i].pi;

            sumIIxy += scalePairs_[i].imu[0]*scalePairs_[i].imu[0] + scalePairs_[i].imu[1]*scalePairs_[i].imu[1];
            sumPPxy += scalePairs_[i].ptam[0]*scalePairs_[i].ptam[0] + scalePairs_[i].ptam[1]*scalePairs_[i].ptam[1];
            sumPIxy += scalePairs_[i].ptam[0]*scalePairs_[i].imu[0] + scalePairs_[i].ptam[1]*scalePairs_[i].imu[1];

            sumIIz += scalePairs_[i].imu[2]*scalePairs_[i].imu[2];
            sumPPz += scalePairs_[i].ptam[2]*scalePairs_[i].ptam[2];
            sumPIz += scalePairs_[i].ptam[2]*scalePairs_[i].imu[2];

            numIn++;
        } else {
            totSumII += scalePairs_[i].ii;
            totSumPP += scalePairs_[i].pp;
            totSumPI += scalePairs_[i].pi;
            numOut++;
        }
    }
    xyz_sum_IMUxIMU = sumII;
    xyz_sum_PTAMxPTAM = sumPP;
    xyz_sum_PTAMxIMU = sumPI;

    double scale_Filtered = scalePairs_[0].computeEstimator(sumPP, sumII, sumPI, 0.2, 0.01);
    double scale_Unfiltered = scalePairs_[0].computeEstimator(sumPP + totSumPP, sumII + totSumII, sumPI + totSumPI, 0.2, 0.01);
    double scale_PTAMSmallVar = scalePairs_[0].computeEstimator(sumPP + totSumPP, sumII + totSumII, sumPI + totSumPI, 0.00001, 1);
    double scale_IMUSmallVar = scalePairs_[0].computeEstimator(sumPP + totSumPP, sumII + totSumII, sumPI + totSumPI, 1, 0.00001);


    double scale_Filtered_xy = scalePairs_[0].computeEstimator(sumPPxy, sumIIxy, sumPIxy, 0.2, 0.01);
    double scale_Filtered_z = scalePairs_[0].computeEstimator(sumPPz, sumIIz, sumPIz, 0.2, 0.01);


    scalePairsIn = numIn;
    scalePairsOut = numOut;

    printf("scale: in: %i; out: %i, filt: %.3f; xyz: %.1f < %.1f < %.1f; xy: %.1f < %.1f < %.1f; z: %.1f < %.1f < %.1f;\n",
            numIn, numOut, scale_Filtered,
            scale_PTAMSmallVar, scale_Unfiltered, scale_IMUSmallVar,
            scalePairs_[0].computeEstimator(sumPPxy, sumIIxy, sumPIxy, 0.00001, 1),
            scale_Filtered_xy,
            scalePairs_[0].computeEstimator(sumPPxy, sumIIxy, sumPIxy, 1, 0.00001),
            scalePairs_[0].computeEstimator(sumPPz, sumIIz, sumPIz, 0.00001, 1),
            scale_Filtered_z,
            scalePairs_[0].computeEstimator(sumPPz, sumIIz, sumPIz, 1, 0.00001)
            );


    if (scale_Filtered > 0.1)
        z_scale = xy_scale = scale_Filtered;
    else
        XCS_LOG_WARN("calculated scale is too small " << scale_Filtered << ", disallowing!");


    scale_from_xy = scale_Filtered_xy;
    scale_from_z = scale_Filtered_z;
    // update offsets such that no position change occurs (X = x_global*xy_scale_old + offset = x_global*xy_scale_new + new_offset)
    if (useScalingFixpoint) {
        // fix at fixpoint
        x_offset += (xyz_scale_old - xy_scale) * scalingFixpoint[0];
        y_offset += (xyz_scale_old - xy_scale) * scalingFixpoint[1];
        z_offset += (xyz_scale_old - z_scale) * scalingFixpoint[2];
    } else {
        // fix at current pos.
        x_offset += (xyz_scale_old - xy_scale) * OrgPtamPose[0];
        y_offset += (xyz_scale_old - xy_scale) * OrgPtamPose[1];
        z_offset += (xyz_scale_old - z_scale) * OrgPtamPose[2];
    }
    scale_xyz_initialized = true;
}

// M: cannot use
//TooN::Vector<3> ScaleEstimation::transformPTAMObservation(double x, double y, double z) {
//    //return transformPTAMObservation(x, y, z, yaw.state[0]); TODO changed
//    return transformPTAMObservation(x, y, z, yaw);
//}

TooN::Vector<3> ScaleEstimation::transformPTAMObservation(double x, double y, double z, double yaw) {
    double yawRad = yaw * 3.14159268 / 180;
    x = x_offset + xy_scale * x - 0.2 * sin(yawRad);
    y = y_offset + xy_scale * y - 0.2 * cos(yawRad);
    z = z_offset + z_scale*z;
    return TooN::makeVector(x, y, z);
}

TooN::Vector<6> ScaleEstimation::transformPTAMObservation(TooN::Vector<6> obs) {
    obs.slice<0, 3>() = transformPTAMObservation(obs[0], obs[1], obs[2], obs[5]);

    obs[3] += roll_offset;
    obs[4] += pitch_offset;
    obs[5] += yaw_offset;

    return obs;
}

/*!
 * Convert EKF state (part of) to PTAM coordinates (i.e. removes scaling)
 */
TooN::Vector<6> ScaleEstimation::backTransformPTAMObservation(TooN::Vector<6> obs) {
    obs[3] -= roll_offset;
    obs[4] -= pitch_offset;
    obs[5] -= yaw_offset;

    double yawRad = obs[5] * 3.14159268 / 180;
    obs[0] = (-x_offset + obs[0] + 0.2 * sin(yawRad)) / xy_scale;
    obs[1] = (-y_offset + obs[1] + 0.2 * cos(yawRad)) / xy_scale;
    obs[2] = (-z_offset + obs[2]) / z_scale;

    return obs;
}