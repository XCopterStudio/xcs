#ifndef SCALES_STRUCT_HPP
#define	SCALES_STRUCT_HPP

#include <TooN/TooN.h>

namespace xcs {
namespace nodes {
namespace localization {

class ScaleStruct {
public:
    TooN::Vector<3> ptam;
    TooN::Vector<3> imu;
    double ptamNorm;
    double imuNorm;
    double alphaSingleEstimate;
    double pp, ii, pi;

    inline double computeEstimator(double spp, double sii, double spi, double stdDevPTAM = 0.2, double stdDevIMU = 0.1) const {
        double sII = stdDevPTAM * stdDevPTAM * sii;
        double sPP = stdDevIMU * stdDevIMU * spp;
        double sPI = stdDevIMU * stdDevPTAM * spi;

        double tmp = (sII - sPP)*(sII - sPP) + 4 * sPI*sPI;
        if (tmp <= 0) tmp = 1e-5; // numeric issues
        return 0.5 * ((sII - sPP) + sqrt(tmp)) / (stdDevPTAM * stdDevPTAM * spi);

    }

    inline ScaleStruct(TooN::Vector<3> ptamDist, TooN::Vector<3> imuDist) {
        ptam = ptamDist;
        imu = imuDist;
        pp = ptam[0] * ptam[0] + ptam[1] * ptam[1] + ptam[2] * ptam[2];
        ii = imu[0] * imu[0] + imu[1] * imu[1] + imu[2] * imu[2];
        pi = imu[0] * ptam[0] + imu[1] * ptam[1] + imu[2] * ptam[2];

        ptamNorm = sqrt(pp);
        imuNorm = sqrt(ii);

        alphaSingleEstimate = computeEstimator(pp, ii, pi);
    }

    inline bool operator<(const ScaleStruct& comp) const {
        return alphaSingleEstimate < comp.alphaSingleEstimate;
    }
};
}
}
}


#endif	/* SCALES_STRUCT_HPP */

