#ifndef PTAM_H
#define PTAM_H

#include "tum/Predictor.h"
#include "tum/scale_estimation.hpp"

#include <TooN/TooN.h>
#include <cvd/image.h>
#include <cvd/byte.h>

#include <memory>
#include <chrono>

class Tracker;
class ATANCamera;
class Map;
class MapMaker;
class GLWindow2;
class MouseKeyHandler;


namespace xcs {
namespace nodes {

namespace localization {

enum PtamStatusType {
    PTAM_IDLE = 0, PTAM_INITIALIZING = 1, PTAM_LOST = 2, PTAM_GOOD = 3, PTAM_BEST = 4, PTAM_TOOKKF = 5, PTAM_FALSEPOSITIVE = 6
};


class Ptam  {
public:
    Ptam();
    virtual ~Ptam();

    void init();

    void handleFrame(urbi::UImage &bwImage, Timestamp timestamp);

private:
    typedef std::unique_ptr<Tracker> TrackerPtr;
    typedef std::unique_ptr<ATANCamera> ATANCameraPtr;
    typedef std::unique_ptr<Map> MapPtr;
    typedef std::unique_ptr<MapMaker> MapMakerPtr;
    typedef std::chrono::high_resolution_clock Clock;
    typedef std::chrono::time_point<Clock> TimePoint;

    static const int FRAME_WIDTH;
    static const int FRAME_HEIGHT;

    TrackerPtr ptamTracker_;
    ATANCameraPtr ptamCamera_;
    MapPtr ptamMap_;
    MapMakerPtr ptamMapMaker_;

    localization::ScaleEstimation scaleEstimation_;

    Predictor* predConvert_; // used ONLY to convert from rpy to se3 and back, i.e. never kept in some state.
    Predictor* imuOnlyPred_;
    Predictor* predIMUOnlyForScale_; // used for scale calculation. needs to be updated with every new navinfo...
    int goodCount_; // number of succ. tracked frames in a row.

    int frameNo_; // frame sequence number TODO use atomic for threaded video update
    CVD::Image<CVD::byte> frame_;
    Timestamp frameTimestamp_;

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


    GLWindow2* glWindow_;
    MouseKeyHandler* glWindowKeyHandler_;



    TooN::Vector<3> evalNavQue(unsigned int from, unsigned int to, bool* zCorrupted, bool* allCorrupted, float* out_start_pressure, float* out_end_pressure);

};

}
}
}

#endif
