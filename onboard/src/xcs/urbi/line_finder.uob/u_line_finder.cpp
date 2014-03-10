#include "u_line_finder.hpp"
#include <thread>
#include <chrono>
#include <cmath>

using namespace xcs::urbi;

const size_t ULineFinder::REFRESH_PERIOD = 100; // ms

ULineFinder::ULineFinder(const std::string& name) :
    ::urbi::UObject(name),
     hasFrame_(false),
     distance_(0) {

    UBindFunction(ULineFinder, init);

    UBindVar(ULineFinder, video);
    UNotifyChange(video, &ULineFinder::onChangeVideo);

    UBindVar(ULineFinder, blurRange);
    UBindVar(ULineFinder, hsvValueRange);
    UBindVar(ULineFinder, cannyT1);
    UBindVar(ULineFinder, cannyT2);
    UBindVar(ULineFinder, cannyApertureSize);
    UBindVar(ULineFinder, cannyL2Gradient);
    UBindVar(ULineFinder, houghRho);
    UBindVar(ULineFinder, houghTheta);
    UBindVar(ULineFinder, houghT);
    UBindVar(ULineFinder, houghMinLength);
    UBindVar(ULineFinder, houghMaxGap);
    UBindVar(ULineFinder, distanceAging);

    UBindVar(ULineFinder, distance);
    UBindVar(ULineFinder, deviation);
    UBindVar(ULineFinder, line);
}

void ULineFinder::init() {
    std::cout << "Initing";
    /*
     * Set default parameters
     */
    blurRange = 5;
    hsvValueRange = 120;
    cannyT1 = 200;
    cannyT2 = 200;
    cannyApertureSize = 3;
    cannyL2Gradient = false;
    houghRho = 4;
    houghTheta = CV_PI / 180;
    houghT = 70;
    houghMinLength = 100;
    houghMaxGap = 40;
    distanceAging = 0.5;
    /*
     * Output vars
     */
    distance = 0;
    deviation = 0;
    line = std::vector<int>(4,0);

    cv::namedWindow("HSV->InRange", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Canny", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Lines", cv::WINDOW_AUTOSIZE);

    USetUpdate(REFRESH_PERIOD);
}

int ULineFinder::update() {
    const static size_t waitDelay = 20;
    processFrame();
    cv::waitKey(waitDelay); // re-render image windows
    // emulate arbitrary processing time
    //std::this_thread::sleep_for(std::chrono::milliseconds((REFRESH_PERIOD - waitDelay) / 2));

    //TODO: return good value
    return -1;
}

void ULineFinder::onChangeVideo(::urbi::UVar& uvar) {
    lastFrame_ = uvar;
    hasFrame_ = true;
    // adapt to different size
    if ((imageHeight_ != lastFrame_.height) || (imageWidth_ != lastFrame_.width)) {
        imageHeight_ = lastFrame_.height;
        imageWidth_ = lastFrame_.width;
        imageCenter_ = cv::Point(lastFrame_.width / 2, lastFrame_.height / 2);
    }

}

void ULineFinder::processFrame() {
    if (!hasFrame_) {
        return;
    }
    /*
     * Image Processing
     */
    cv::Mat src(lastFrame_.height, lastFrame_.width, CV_8UC3, lastFrame_.data);
    cv::Mat mid;

    /*
     * 1. Denoise
     */
    cv::GaussianBlur(src, mid, cv::Size(static_cast<int> (blurRange), static_cast<int> (blurRange)), 0, 0);

    /*
     * 2. Differentiate line pixels from others
     */
    // convert to HSV
    cv::cvtColor(mid, mid, CV_BGR2HSV);
    // highlight ROI (region of interest)
    cv::inRange(mid, cv::Scalar(0, 0, 0), cv::Scalar(255, 255, static_cast<double> (hsvValueRange)), mid);
    cv::imshow("HSV->InRange", mid);
    // transform to edge map
    cv::Canny(mid, mid, static_cast<double> (cannyT1), static_cast<double> (cannyT2), static_cast<int> (cannyApertureSize), static_cast<double> (cannyL2Gradient));
    cv::imshow("Canny", mid);
    /*
     * 3. Detect lines
     */
    cv::vector<cv::Vec4i> lines;
    cv::HoughLinesP(mid, lines, static_cast<double> (houghRho), static_cast<double> (houghTheta), static_cast<int> (houghT), static_cast<double> (houghMinLength), static_cast<double> (houghMaxGap));

    /*
     * 4. Compute single vector from lines
     */
    cv::Vec4i avg = cv::mean(lines);

    /*
     * Compute output vars
     */

    // find distance from the middle
    double dev(distance_); // use previous deviation
    double devAging = static_cast<double> (distanceAging);
    cv::Point norm(avg[3] - avg[1], avg[0] - avg[2]);
    cv::Scalar color;
    if (norm.x == 0 && norm.y == 0) {
        color = cv::Scalar(0, 128, 128);
    }
    else {
        auto c = -norm.dot(cv::Point(avg[0], avg[1]));
        distance_ = dev = (1 - devAging) * dev + devAging * ((norm.dot(imageCenter_) + c) / hypot(norm.x, norm.y)); // weighted average of current and previous deviation
        color = (dev > 0) ? cv::Scalar(0, 255, 255) : cv::Scalar(0, 255, 0);
    }

    /*
     * Output vars
     */
    std::vector<int> l = std::vector<int>(4);
    l[0] = avg[0];
    l[1] = avg[1];
    l[2] = avg[2];
    l[3] = avg[3];
    line = l;

    distance = distance_;

    if (norm.x != 0) {
        deviation = atan(norm.y/norm.x);
    }


    /*
     * Display annotated frame
     */
    for (auto l : lines) {
        cv::line(src, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(255, 0, 0), 2, CV_AA);
    }
    cv::circle(src, imageCenter_, abs(dev), color, 3, CV_AA);
    cv::line(src, cv::Point(avg[0], avg[1]), cv::Point(avg[2], avg[3]), cv::Scalar(0, 0, 255), 3, CV_AA);
    cv::circle(src, cv::Point(avg[2], avg[3]), 5, cv::Scalar(0, 0, 255), 3, CV_AA);
    cv::imshow("Lines", src);

}

UStart(ULineFinder);
