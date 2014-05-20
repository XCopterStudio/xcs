#ifndef X_PTAM_H
#define X_PTAM_H

#include "tum/Predictor.h"

#include <xcs/nodes/xobject/x_object.hpp>
#include <xcs/nodes/xobject/x_input_port.hpp>

#include <ptam/Tracker.h>
#include <ptam/ATANCamera.h>
#include <ptam/Map.h>
#include <ptam/MapMaker.h>
#include <ptam/GLWindow2.h>
#include <ptam/MouseKeyHandler.h>
#include <TooN/TooN.h>

#include <memory>
#include <chrono>

namespace xcs {
namespace nodes {

namespace ptam {

enum PtamStatusType {
    PTAM_IDLE = 0, PTAM_INITIALIZING = 1, PTAM_LOST = 2, PTAM_GOOD = 3, PTAM_BEST = 4, PTAM_TOOKKF = 5, PTAM_FALSEPOSITIVE = 6
};
}

class XPtam : public XObject, private MouseKeyHandler {
public:
    XPtam(const std::string &name);
    virtual ~XPtam();

    xcs::nodes::XInputPort<::urbi::UImage> video;

    void init();
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
    
    ptam::PtamStatusType ptamStatus_; // XVar candidate


    GLWindow2* glWindow_;



    void onChangeVideo(::urbi::UImage image);

    TooN::Vector<3> evalNavQue(unsigned int from, unsigned int to, bool* zCorrupted, bool* allCorrupted, float* out_start_pressure, float* out_end_pressure);

};

}
}

#endif
