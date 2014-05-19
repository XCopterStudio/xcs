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

#include <memory>

namespace xcs {
namespace nodes {

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
    
    static const int FRAME_WIDTH;
    static const int FRAME_HEIGHT;

    TrackerPtr ptamTracker_;
    ATANCameraPtr ptamCamera_;
    MapPtr ptamMap_;
    MapMakerPtr ptamMapMaker_;

    Predictor* predConvert_; // used ONLY to convert from rpy to se3 and back, i.e. never kept in some state.

    GLWindow2* glWindow_;



    void onChangeVideo(::urbi::UImage image);
};

}
}

#endif
