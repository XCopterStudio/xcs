#ifndef TUMUTILS_HPP
#define	TUMUTILS_HPP

#include <TooN/TooN.h>
#include <TooN/se3.h>
#include <TooN/so3.h>
#include <chrono>
#include <cmath>

namespace xcs {
namespace nodes {
namespace localization {

typedef TooN::SE3<double> SE3Element;

inline SE3Element vectorToSe3(const TooN::Vector<6> &pose) {
    SE3Element result(TooN::Identity, pose.slice<0, 3>());

    const double sPhi = sin(pose[3]);
    const double cPhi = cos(pose[3]);
    const double sThe = sin(pose[4]);
    const double cThe = cos(pose[4]);
    const double sPsi = sin(pose[5]);
    const double cPsi = cos(pose[5]);

    TooN::Matrix<3, 3> mat;
    mat(0, 0) = cPsi * cPhi + sPsi * sThe * sPhi;
    mat(0, 1) = sPsi * cThe;
    mat(0, 2) = cPsi * sPhi - cPhi * sPsi * sThe;

    mat(1, 0) = -sPsi * cPhi + cPsi * sThe * sPhi;
    mat(1, 1) = cPsi * cThe;
    mat(1, 2) = -sPsi * sPhi - cPsi * sThe * cPhi;

    mat(2, 0) = -cThe * sPhi;
    mat(2, 1) = sThe;
    mat(2, 2) = cThe * cPhi;

    result.get_rotation() = mat;

    return result;
}

inline TooN::Vector<3> so3ToAngles(const TooN::SO3<> &pose) {
    TooN::Vector<3> result;
    auto mat = pose.get_matrix();

    result[0] = atan2(-mat(2, 0), mat(2, 2)); // phi
    result[1] = atan2(mat(2, 1), hypot(mat(2, 0), mat(2, 2))); // theta
    result[2] = atan2(mat(0, 1), mat(1, 1)); // psi

    return result;
}

template<typename RepT, typename DurationT>
inline static double duration2sec(const std::chrono::duration<RepT, DurationT> &duration) {
    return (double) std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count() / 1e9;
}
}
}
}

#endif	/* TUMUTILS_HPP */

