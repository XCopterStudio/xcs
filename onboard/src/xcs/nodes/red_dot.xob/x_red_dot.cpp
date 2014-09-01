#include "x_red_dot.hpp"
#include <xcs/nodes/xobject/x.h>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace xcs;
using namespace xcs::nodes::reddot;

void XRedDot::onChangeVideo(::urbi::UImage image){
    if (stopped_){
        return;
    }

    if (image.height != lastImage.rows || image.width != lastImage.cols){
        lastImage.create(image.height, image.width, CV_8UC3);
    }
    //cv::Mat imageCV(image.height, image.width, CV_8UC3, image.data);
    memcpy(lastImage.data, image.data, image.height*image.width * 3);

    Mat out;
    bool foundDot = redDot_.findRedDot(lastImage, out);
    found = foundDot;

    urbi::UImage bin;
    bin.width = image.width;
    bin.height = image.height;
    bin.size = image.width * image.height * 3;
    bin.imageFormat = urbi::IMAGE_RGB;
    bin.data = out.data;
    enhancedVideo = bin;

    if(foundDot){
        ImagePosition center = redDot_.lastPosition();
        errorX = (image.width / 2.0) - center.x;
        errorY = center.y - (image.height/2.0);
    } 
}

void XRedDot::stateChanged(XObject::State state){
    switch (state) {
    case XObject::STATE_STARTED:
        stopped_ = false;
        break;
    case XObject::STATE_STOPPED:
        stopped_ = true;
        break;
    }
}

//=============== public functions ===============

XRedDot::XRedDot(const std::string &name) : XObject(name),
    video("CAMERA"), enhancedVideo("CAMERA"), found("DECISION"), errorX("PID_ERROR"),
    errorY("PID_ERROR"){

    XBindVar(video);
    UNotifyThreadedChange(video.data(), &XRedDot::onChangeVideo, urbi::LOCK_FUNCTION_DROP);

    XBindVar(enhancedVideo);
    XBindVar(found);
    XBindVar(errorX);
    XBindVar(errorY);
}

XStart(XRedDot);