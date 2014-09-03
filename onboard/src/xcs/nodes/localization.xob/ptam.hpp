#ifndef PTAM_H
#define PTAM_H

#include "tum/Predictor.h"
#include "tum/scale_estimation.hpp"
#include "ekf.hpp"

#include <xcs/types/timestamp.hpp>

#include <TooN/TooN.h>
#include <cvd/image.h>
#include <cvd/byte.h>

#include <urbi/uimage.hh>

#include <memory>
#include <chrono>
#include <mutex>
#include <vector>
#include <map>
#include <atomic>

class Tracker;
class ATANCamera;
class Map;
class MapMaker;
class GLWindow2;
#include <ptam/MouseKeyHandler.h> // class MouseKeyHandler;


namespace xcs {
namespace nodes {

namespace localization {

enum PtamStatusType {
    PTAM_IDLE = 0,
    PTAM_INITIALIZING = 1,
    PTAM_LOST = 2,
    PTAM_GOOD = 3,
    PTAM_BEST = 4,
    PTAM_TOOKKF = 5,
    PTAM_FALSEPOSITIVE = 6,
    PTAM_DISABLED = 7
};

class Ptam : public MouseKeyHandler {
public:
    typedef std::vector<double> CameraParameters;
    typedef std::map<std::string, double> Parameters;

    Ptam(Ekf &ekf);
    virtual ~Ptam();

    /*!
     * \param timestamp Timestamp in EKF's time.
     */
    void handleFrame(::urbi::UImage &bwImage, Timestamp timestamp);

    /*!
     * \param timestamp EKF time.
     */
    void measurementImu(const DroneStateMeasurement measurement, const double timestamp);

    void cameraParameters(const CameraParameters& values);

    void parameters(const Parameters& values);

    inline localization::PtamStatusType ptamStatus() {
        return ptamStatus_;
    }

    /*
     * Control functions
     */
    void takeInitKF();

    void takeKF();

    /*!
     * \note handleFrame must be called to take effect of the reset, i.e. PTAM won't
     * be reset until PTAM is enabled (in XLocalization).
     */
    void reset();


private:
    typedef std::unique_ptr<Tracker> TrackerPtr;
    typedef std::unique_ptr<ATANCamera> ATANCameraPtr;
    typedef std::unique_ptr<Map> MapPtr;
    typedef std::unique_ptr<MapMaker> MapMakerPtr;
    typedef std::unique_ptr<GLWindow2> GLWindow2Ptr;
    typedef std::chrono::high_resolution_clock Clock;
    typedef std::chrono::time_point<Clock> TimePoint;

    /*
     * Configuration
     */
    CameraParameters cameraParameters_;
    Parameters parameters_;
    bool showWindow_;


    TrackerPtr ptamTracker_;
    ATANCameraPtr ptamCamera_;
    MapPtr ptamMap_;
    MapMakerPtr ptamMapMaker_;

    localization::ScaleEstimation scaleEstimation_;

    /*!
     * Used only for scale calculation.
     */
    Predictor predIMUOnlyForScale_; 
    /*!
     * >= 0 no. of succesfully tracked frames in a row
     * <= 0 no. of unsuccesfully tracked frames in a row
     */
    int goodCount_;
    /*!
     * No. of observations that were used by EKF.
     */
    int goodObservations_;

    int frameNo_;
    CVD::Image<CVD::byte> frame_;

    std::atomic<bool> resetPtamRequested_;
    std::atomic<bool> ptamReady_;
    bool mapLocked_; // XVar candidate
    bool forceKF_; // XVar candidate
    int maxKF_; // XVar candidate

    /*!
     * -1    scale estimate acquisition not active
     * >= 0  no. of frames for scale estimate interval
     */
    int framesForScaleInterval_;
    bool updateFixpoint_;
    TimePoint lastGoodYawClock_;

    TooN::Vector<3> lastPtamPositionForScale_;
    Timestamp ptamPositionForScaleTakenTimestamp_;
    Timestamp lastScaleEKFtimestamp_;

    localization::PtamStatusType ptamStatus_; // XVar candidate


    GLWindow2Ptr glWindow_;

    Ekf &ekf_;

    localization::ImuMeasurements imuMeasurements_;
    std::mutex imuMeasurementsMtx_;

    TooN::Vector<3> evalNavQue(xcs::Timestamp from, xcs::Timestamp to, bool* zCorrupted, bool* allCorrupted);

    void resetPtam();

    void createPtam();

};

}
}
}

#endif
