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
    PTAM_IDLE = 0, PTAM_INITIALIZING = 1, PTAM_LOST = 2, PTAM_GOOD = 3, PTAM_BEST = 4, PTAM_TOOKKF = 5, PTAM_FALSEPOSITIVE = 6
};

class Ptam : public MouseKeyHandler {
public:
    Ptam(Ekf &ekf);
    virtual ~Ptam();

    /*!
     * \param timestamp Timestamp in EKF's time.
     */
    void handleFrame(::urbi::UImage &bwImage, Timestamp timestamp);

    /*!
     * \param timestamp EKF time.
     */
    void measurementImu(const DroneStateMeasurement &measurement, const double &timestamp);

    virtual void on_key_down(int key);
    
    void pressSpacebar();
private:
    typedef std::unique_ptr<Tracker> TrackerPtr;
    typedef std::unique_ptr<ATANCamera> ATANCameraPtr;
    typedef std::unique_ptr<Map> MapPtr;
    typedef std::unique_ptr<MapMaker> MapMakerPtr;
    typedef std::unique_ptr<GLWindow2> GLWindow2Ptr;
    typedef std::unique_ptr<MouseKeyHandler> MouseKeyHandlerPtr;
    typedef std::chrono::high_resolution_clock Clock;
    typedef std::chrono::time_point<Clock> TimePoint;

    /*! Default frame size */
    static const int FRAME_WIDTH;
    static const int FRAME_HEIGHT;

    TooN::Vector<5> cameraParameters_;

    TrackerPtr ptamTracker_;
    ATANCameraPtr ptamCamera_;
    MapPtr ptamMap_;
    MapMakerPtr ptamMapMaker_;

    localization::ScaleEstimation scaleEstimation_;

    Predictor predConvert_; // used ONLY to convert from rpy to se3 and back, i.e. never kept in some state.
    Predictor imuOnlyPred_; // TODO navdata should be fed to this predictor (M: IMO unnecessary)
    Predictor predIMUOnlyForScale_; // used for scale calculation. needs to be updated with every new navinfo..., // TODO navdata should be fed to this predictor
    int goodCount_; // number of succ. tracked frames in a row.
    int goodObservations_; // another metric of tracking quality (TODO explain better)

    int frameNo_; // frame sequence number TODO use atomic for threaded video update
    CVD::Image<CVD::byte> frame_;

    bool resetPtamRequested_;
    bool mapLocked_; // XVar candidate
    bool forceKF_; // XVar candidate
    int maxKF_; // XVar candidate

    int framesIncludedForScaleXYZ_;
    bool lockNextFrame_;
    TimePoint lastGoodYawClock_;

    TooN::Vector<3> ptamPositionForScale_;
    Timestamp ptamPositionForScaleTakenTimestamp_;
    Timestamp lastScaleEKFtimestamp_;

    localization::PtamStatusType ptamStatus_; // XVar candidate


    GLWindow2Ptr glWindow_;
    MouseKeyHandlerPtr glWindowKeyHandler_;

    Ekf &ekf_;

    localization::ImuMeasurements imuMeasurements_;


    TooN::Vector<3> evalNavQue(xcs::Timestamp from, xcs::Timestamp to, bool* zCorrupted, bool* allCorrupted);

    void resetPtam();

};

}
}
}

#endif
