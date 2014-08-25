#include "x_red_dot.hpp"
#include <xcs/nodes/xobject/x.h>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace xcs;
using namespace xcs::nodes::reddot;

void XRedDot::onChangeVideo(::urbi::UImage image){
    cv::Mat imageCV(image.height, image.width, CV_8UC3, image.data);
    ImagePosition position = redDot_.findRedDot(imageCV);

    if (position.x >= 0 && position.y >= 0){
        errorX = position.x;
        errorY = position.y;
        found = true;
    }
    else{
        found = false;
    }
}

//=============== public functions ===============

XRedDot::XRedDot(const std::string &name) : XObject(name),
    video("CAMERA"), found("DECISION"), errorX("PID_ERROR"),
    errorY("PID_ERROR"){
    XBindVarF(video, &XRedDot::onChangeVideo);

    XBindVar(found);
    XBindVar(errorX);
    XBindVar(errorY);
}

XStart(XRedDot);