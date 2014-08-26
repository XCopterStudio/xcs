#include "red_dot.hpp"
#include <cinttypes>

using namespace cv;
using namespace xcs::nodes::reddot;

void RedDot::getRedMask(const cv::Mat &image, cv::Mat &redMask){
    Mat mask;

    Scalar lower_red(115, 100, 100);
    Scalar upper_red(125, 200, 200);
    inRange(image, lower_red, upper_red, redMask);
}

//================ public functions =========

ImagePosition RedDot::findRedDot(const cv::Mat &image, cv::Mat &out){
    Mat imageHSV;
    cvtColor(image, imageHSV, CV_RGB2HSV);

    Mat redMask;
    //cvtColor(image, redMask, CV_RGB2GRAY);
    getRedMask(imageHSV,redMask);
    GaussianBlur(redMask, redMask, Size(9, 9), 2, 2);
    //cvtColor(redMask, out, CV_GRAY2RGB);

    vector<Vec3f> circles;
    HoughCircles(redMask, circles, CV_HOUGH_GRADIENT,
        4, redMask.cols, 200, 100, 8);

    image.copyTo(out);
    for (size_t i = 0; i < circles.size(); i++)
    {
        Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
        int radius = cvRound(circles[i][2]);
        // draw the circle center
        circle(out, center, 3, Scalar(0, 255, 0), -1, 8, 0);
        // draw the circle outline
        circle(out, center, radius, Scalar(0, 0, 255), 3, 8, 0);
    }

    if (!circles.empty()){
        Point center(cvRound(circles[0][0]), cvRound(circles[0][1]));
        return ImagePosition(center.x,center.y);
    } else {
        return ImagePosition(-1, -1);
    }
}
