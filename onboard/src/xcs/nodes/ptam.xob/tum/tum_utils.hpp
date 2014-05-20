#ifndef TUMUTILS_HPP
#define	TUMUTILS_HPP

#include <TooN/TooN.h>
#include <TooN/so3.h>
#include <chrono>

namespace xcs {
namespace nodes {
namespace ptam {

class TumUtils {
private:
    /* TODO Those are only dummy values to have compilable/linkable code.
     They should be properly obtained from EKF.
     */
    constexpr static double yaw = 0;
    
    constexpr static double x_offset = 0;
    constexpr static double y_offset = 0;
    constexpr static double z_offset = 0;
    
    constexpr static double roll_offset = 0;
    constexpr static double pitch_offset = 0;
    constexpr static double yaw_offset = 0;
    
    constexpr static double xy_scale = 1;
    constexpr static double z_scale = 1;
public:

    static TooN::Vector<3> transformPTAMObservation(double x, double y, double z);
    static TooN::Vector<3> transformPTAMObservation(double x, double y, double z, double yaw);
    static TooN::Vector<6> transformPTAMObservation(TooN::Vector<6> obs);
    static TooN::Vector<6> backTransformPTAMObservation(TooN::Vector<6> obs);
};

/****************************************************
 *********** Pose-Representation Conversion **********
 ****************************************************/
// SEEE http://de.wikipedia.org/wiki/Roll-Nick-Gier-Winkel

// the drone coordinate system is:
// x-axis: to the left
// y-axis: up
// z-axis: forward
// roll rhs-system correnct
// pitch: rhs-system *-1;
// yaw: rhs system *-1;

inline static TooN::SO3<> rpy2rod(double roll, double pitch, double yaw) {
    TooN::Matrix<3, 3> mat;

    pitch /= 180 / 3.14159265;
    roll /= 180 / 3.14159265;
    yaw /= -180 / 3.14159265;

    double sa = sin(yaw); // a is yaw = psi
    double ca = cos(yaw);
    double sb = sin(roll); // b is roll = phi
    double cb = cos(roll);
    double sg = sin(pitch); // g is pitch = theta
    double cg = cos(pitch);

    mat(0, 0) = ca*cb;
    mat(0, 1) = sa*cb;
    mat(0, 2) = -sb;

    mat(1, 0) = ca * sb * sg - sa*cg;
    mat(1, 1) = sa * sb * sg + ca*cg;
    mat(1, 2) = cb*sg;

    mat(2, 0) = ca * sb * cg + sa*sg;
    mat(2, 1) = sa * sb * cg - ca*sg;
    mat(2, 2) = cb*cg;

    //mat = mat.T();

    TooN::SO3<> res = mat;
    return res.inverse();
}

inline static void rod2rpy(TooN::SO3<> trans, double* roll, double* pitch, double* yaw) {
    TooN::Matrix<3, 3> mat = trans.inverse().get_matrix(); //.T();

    *roll = atan2(-mat(0, 2), sqrt(mat(0, 0) * mat(0, 0) + mat(0, 1) * mat(0, 1)));
    *yaw = atan2(mat(0, 1) / cos(*roll), mat(0, 0) / cos(*roll));
    *pitch = atan2(mat(1, 2) / cos(*roll), mat(2, 2) / cos(*roll));

    *pitch *= 180 / 3.14159265;
    *roll *= 180 / 3.14159265;
    *yaw *= -180 / 3.14159265;


    while (*pitch > 180) *pitch -= 360;
    while (*pitch < -180) *pitch += 360;
    while (*roll > 180) *roll -= 360;
    while (*roll < -180) *roll += 360;
    while (*yaw > 180) *yaw -= 360;
    while (*yaw < -180) *yaw += 360;
}

template<typename RepT, typename DurationT>
inline static double duration2sec(const std::chrono::duration<RepT, DurationT> &duration) {
    return (double) std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count() / 1e9;
}
}
}
}

#endif	/* TUMUTILS_HPP */
