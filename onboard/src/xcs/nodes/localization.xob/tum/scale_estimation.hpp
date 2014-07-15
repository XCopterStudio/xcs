#ifndef SCALEESTIMATION_HPP
#define	SCALEESTIMATION_HPP

#include "scale_struct.hpp"
#include "tum_utils.hpp"
#include <TooN/TooN.h>

namespace xcs {
namespace nodes {
namespace localization {

class ScaleEstimation {
public:
    ScaleEstimation();

    inline const SE3Element& offsetMatrix() const {
        return offsetMatrix_;
    }

    inline double scale() const {
        return scale_;
    }

    void initializeScale(const double scale);

    void initializeOffset(const SE3Element& offset);

    void updateScale(const TooN::Vector<3> ptamDiff, const TooN::Vector<3> imuDiff, const TooN::Vector<3> ptamPose);

    void scalingFixpoint(const TooN::Vector<3> scalingFixpoint) {
        scalingFixpoint_ = scalingFixpoint;
    }

    inline const TooN::Vector<3>& scalingFixpoint() const {
        return scalingFixpoint_;
    }

    /*
     * Transformations (rewrite to config for other drones)
     */
    static const SE3Element droneToFront;

private:
    typedef std::vector<ScaleStruct> ScalePairsType;

    bool useScalingFixpoint_;
    TooN::Vector<3> scalingFixpoint_;

    double scale_;
    double initialScale_;

    bool offsetInitialized_;

    SE3Element offsetMatrix_;


    // intermediate values for re-estimation of relaton parameters
    double xyz_sum_PTAMxIMU_; // TODO potential use for scale accuracy (or precision?)
    ScalePairsType scalePairs_;
};

}
}
}

#endif	/* SCALEESTIMATION_HPP */

