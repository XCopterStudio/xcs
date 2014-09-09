#ifndef X_RED_DOT_H
#define X_RED_DOT_H

#include <xcs/xcs_fce.hpp>
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
        void onChangeLowerH(int value){ redDot_.lowerH(valueInRange(value, 0, 179)); };
        void onChangeUpperH(int value){ redDot_.upperH(valueInRange(value, 0, 179)); };
        void onChangeLowerS(int value){ redDot_.lowerS(valueInRange(value, 0, 255)); };
        void onChangeUpperS(int value){ redDot_.upperS(valueInRange(value, 0, 255)); };
        void onChangeLowerV(int value){ redDot_.lowerV(valueInRange(value, 0, 255)); };
        void onChangeUpperV(int value){ redDot_.upperV(valueInRange(value, 0, 255)); };
    protected:
        virtual void stateChanged(XObject::State state);
    public:
        XInputPort<int> lowerH;
        XInputPort<int> upperH;
        XInputPort<int> lowerS;
        XInputPort<int> upperS;
        XInputPort<int> lowerV;
        XInputPort<int> upperV;
        XInputPort<::urbi::UImage> video;

        XVar<::urbi::UImage> enhancedVideo;
        XVar<bool> found;
        XVar<double> errorX;
        XVar<double> errorY;

        XRedDot(const std::string &name);
    };

}}}

#endif 