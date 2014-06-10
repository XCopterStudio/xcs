#ifndef SCALEESTIMATION_HPP
#define	SCALEESTIMATION_HPP

#include "scale_struct.hpp"
#include <TooN/TooN.h>

namespace xcs {
namespace nodes {
namespace localization {

class ScaleEstimation {
public:
    ScaleEstimation();
    
    TooN::Vector<6> getCurrentOffsets();
    
    TooN::Vector<3> getCurrentScales();
    
    void setCurrentScales(TooN::Vector<3> scales);

    void updateScaleXYZ(TooN::Vector<3> ptamDiff, TooN::Vector<3> imuDiff, TooN::Vector<3> OrgPtamPose);

    //TooN::Vector<3> transformPTAMObservation(double x, double y, double z);
    TooN::Vector<3> transformPTAMObservation(double x, double y, double z, double yaw);
    TooN::Vector<6> transformPTAMObservation(TooN::Vector<6> obs);
    TooN::Vector<6> backTransformPTAMObservation(TooN::Vector<6> obs);

    // when scaling factors are updated, exacly one point stays the same.
    // if useScalingFixpoint, this point is the current PTAM pose, otherwise it is sclingFixpoint (which is a PTAM-coordinate(!))
    TooN::Vector<3> scalingFixpoint; // in PTAM's system (!)
    bool useScalingFixpoint;
    bool allSyncLocked;
private:
    typedef std::vector<ScaleStruct> ScalePairsType;

    // relation parameters (ptam to imu scale / offset)
    bool offsets_xyz_initialized;
    bool scale_xyz_initialized;
    double xy_scale, z_scale;
    double scale_from_xy;
    double scale_from_z;
    double roll_offset, pitch_offset, yaw_offset;
    double x_offset, y_offset, z_offset;

    // intermediate values for re-estimation of relaton parameters
    double xyz_sum_IMUxIMU;
    double xyz_sum_PTAMxPTAM;
    double xyz_sum_PTAMxIMU;
    double rp_offset_framesContributed;
    ScalePairsType scalePairs_; // TODO change to stack allocation

    double initialScaleSet; // M: ?

    int scalePairsIn, scalePairsOut;

};

}
}
}

#endif	/* SCALEESTIMATION_HPP */

