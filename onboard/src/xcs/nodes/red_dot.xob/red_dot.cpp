#include "red_dot.hpp"
#include <cinttypes>

using namespace cv;
using namespace xcs::nodes::reddot;

void RedDot::getRedMask(const cv::Mat &image, cv::Mat &redMask){
    Scalar lower_red(190,0,0);
    Scalar upper_red(256,256,256);
    inRange(image, lower_red, upper_red, redMask);
}

//================ public functions =========

ImagePosition RedDot::findRedDot(const cv::Mat image){
    Mat imageHSV;

    cvtColor(image, imageHSV, CV_RGB2HSV);
    imshow("Original image", image);

    Mat redMask;
    getRedMask(image,redMask);

    imshow("Red mask", redMask);

    return ImagePosition(-1, -1);
}
