#include "scale_estimation.hpp"

#include <xcs/logging.hpp>
#include <algorithm>

using namespace xcs::nodes::localization;

const SE3Element ScaleEstimation::droneToFront = SE3Element(TooN::SO3<double>(TooN::makeVector(3.14159265 / 2, 0, 0)), TooN::makeVector(0, -0.025, -0.2));

ScaleEstimation::ScaleEstimation() :
  useScalingFixpoint_(false),
  scale_(1), initialScale_(1),
  offsetInitialized_(false) {

}

void ScaleEstimation::initializeScale(const double scale) {
    initialScale_ = scale_ = scale;
    offsetMatrix_ = SE3Element();
    offsetInitialized_ = false;

    /* Michal: some magic constants (0.2) from TUM. */
    xyz_sum_PTAMxIMU_ = 0.2;

    scalePairs_.clear();

    scalePairs_.push_back(ScaleStruct(
            TooN::makeVector(0.2, 0.2, 0.2) / sqrt(scale),
            TooN::makeVector(0.2, 0.2, 0.2) * sqrt(scale)
            ));

}

void ScaleEstimation::initializeOffset(const SE3Element& offset) {
    if (!offsetInitialized_) { // TODO obtain both matrices and calculate on demand (and what about anles averaging?)
        offsetMatrix_ = offset;
        offsetInitialized_ = true;
    }
}

void ScaleEstimation::updateScale(const TooN::Vector<3> ptamDiff, const TooN::Vector<3> imuDiff, const TooN::Vector<3> ptamPose) {
    ScaleStruct s = ScaleStruct(ptamDiff, imuDiff);

    // dont add samples that are way to small...
    if (s.imuNorm < 0.05 || s.ptamNorm < 0.05) return;


    // update running sums
    scalePairs_.push_back(s);

    double scaleOld = scale_;


    // find median.
    std::sort(scalePairs_.begin(), scalePairs_.end());
    double median = scalePairs_[(scalePairs_.size() + 1) / 2].alphaSingleEstimate;

    // hack: if we have only few samples, median is unreliable (maybe 2 out of 3 are completely wrong.
    // so take first scale pair in this case (i.e. the initial scale)
    if (scalePairs_.size() < 5) {
        median = initialScale_;
    }

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

            sumIIxy += scalePairs_[i].imu[0] * scalePairs_[i].imu[0] + scalePairs_[i].imu[1] * scalePairs_[i].imu[1];
            sumPPxy += scalePairs_[i].ptam[0] * scalePairs_[i].ptam[0] + scalePairs_[i].ptam[1] * scalePairs_[i].ptam[1];
            sumPIxy += scalePairs_[i].ptam[0] * scalePairs_[i].imu[0] + scalePairs_[i].ptam[1] * scalePairs_[i].imu[1];

            sumIIz += scalePairs_[i].imu[2] * scalePairs_[i].imu[2];
            sumPPz += scalePairs_[i].ptam[2] * scalePairs_[i].ptam[2];
            sumPIz += scalePairs_[i].ptam[2] * scalePairs_[i].imu[2];

            numIn++;
        } else {
            totSumII += scalePairs_[i].ii;
            totSumPP += scalePairs_[i].pp;
            totSumPI += scalePairs_[i].pi;
            numOut++;
        }
    }
    xyz_sum_PTAMxIMU_ = sumPI;

    double scale_Filtered = scalePairs_[0].computeEstimator(sumPP, sumII, sumPI, 0.2, 0.01);
    double scale_Unfiltered = scalePairs_[0].computeEstimator(sumPP + totSumPP, sumII + totSumII, sumPI + totSumPI, 0.2, 0.01);
    double scale_PTAMSmallVar = scalePairs_[0].computeEstimator(sumPP + totSumPP, sumII + totSumII, sumPI + totSumPI, 0.00001, 1);
    double scale_IMUSmallVar = scalePairs_[0].computeEstimator(sumPP + totSumPP, sumII + totSumII, sumPI + totSumPI, 1, 0.00001);


    double scale_Filtered_xy = scalePairs_[0].computeEstimator(sumPPxy, sumIIxy, sumPIxy, 0.2, 0.01);
    double scale_Filtered_z = scalePairs_[0].computeEstimator(sumPPz, sumIIz, sumPIz, 0.2, 0.01);

    if (scale_Filtered > 0.1) {
        scale_ = scale_Filtered;
    } else {
        XCS_LOG_WARN("calculated scale is too small " << scale_Filtered << ", disallowing!");
    }

    // update offsets such that no position change occurs (X = x_global*xy_scale_old + offset = x_global*xy_scale_new + new_offset)
    const auto &fixpoint = useScalingFixpoint_ ? scalingFixpoint_ : ptamPose;
    offsetMatrix_.get_translation() += (scaleOld - scale_) * (offsetMatrix_.get_rotation() * fixpoint);
}
