#ifndef X_RED_DOT_H
#define X_RED_DOT_H

#include <xcs/nodes/xobject/x_object.hpp>
#include <xcs/nodes/xobject/x_input_port.hpp>
#include <xcs/nodes/xobject/x_var.hpp>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "red_dot.hpp"

#include <atomic>

namespace xcs{
namespace nodes{
namespace reddot{

    class XRedDot : public XObject{
        cv::Mat lastImage;
        RedDot redDot_;

        std::atomic<bool> stopped_;

        void onChangeVideo(::urbi::UImage image);
    protected:
        virtual void stateChanged(XObject::State state);
    public:
        XInputPort<::urbi::UImage> video;

        XVar<::urbi::UImage> enhancedVideo;
        XVar<bool> found;
        XVar<double> errorX;
        XVar<double> errorY;

        XRedDot(const std::string &name);
    };

}}}

#endif 