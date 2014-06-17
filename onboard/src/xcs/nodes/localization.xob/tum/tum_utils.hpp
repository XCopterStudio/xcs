#ifndef TUMUTILS_HPP
#define	TUMUTILS_HPP

#include <TooN/TooN.h>
#include <TooN/so3.h>
#include <chrono>
#include <cmath>

namespace xcs {
namespace nodes {
namespace localization {

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

inline static TooN::SO3<> rpy2rod(const double roll, const double pitch, const double yaw) {
    TooN::Matrix<3, 3> mat;

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

    while (*pitch > M_PI) *pitch -= 2 * M_PI;
    while (*pitch < -M_PI) *pitch += 2 * M_PI;
    while (*roll > M_PI) *roll -= 2 * M_PI;
    while (*roll < -M_PI) *roll += 2 * M_PI;
    while (*yaw > M_PI) *yaw -= 2 * M_PI;
    while (*yaw < -M_PI) *yaw += 2 * M_PI;
}

template<typename RepT, typename DurationT>
inline static double duration2sec(const std::chrono::duration<RepT, DurationT> &duration) {
    return (double) std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count() / 1e9;
}
}
}
}

#endif	/* TUMUTILS_HPP */

