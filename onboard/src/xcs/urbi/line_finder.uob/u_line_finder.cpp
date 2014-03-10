#include "u_line_finder.hpp"
#include <thread>
#include <chrono>
#include <cmath>

using namespace xcs::urbi;
using namespace std;

const size_t ULineFinder::REFRESH_PERIOD = 100; // ms

ULineFinder::ULineFinder(const string& name) :
  ::urbi::UObject(name),
  hasFrame_(false),
  lastReceivedFrameNo_(1), // must be greater than lastProcessedFrame_ at the beginning
  lastProcessedFrameNo_(0),
  distance_(0) {
    UBindFunction(ULineFinder, init);

    UBindVar(ULineFinder, video);
    UNotifyChange(video, &ULineFinder::onChangeVideo);

    UBindVar(ULineFinder, blurRange);
    UBindVar(ULineFinder, autoHsvValueRange);
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
    cout << "Initing";
    /*
     * Set default parameters
     */
    blurRange = 5;
    autoHsvValueRange = false; //TODO change to true when debugged
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
    line = vector<int>(4, 0);

    cv::namedWindow("HSV->InRange", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Canny", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Lines", cv::WINDOW_AUTOSIZE);

    USetUpdate(REFRESH_PERIOD);
}

int ULineFinder::update() {
    const static size_t waitDelay = 20;
    processFrame();
    cv::waitKey(waitDelay); // re-render image windows

    return 0; // Urbi undocumented, return value probably doesn't matter
}

void ULineFinder::onChangeVideo(::urbi::UVar& uvar) {
    lastFrame_ = uvar;
    hasFrame_ = true;
    lastReceivedFrameNo_ += 1;
    // adapt to different size
    if ((imageHeight_ != lastFrame_.height) || (imageWidth_ != lastFrame_.width)) {
        imageHeight_ = lastFrame_.height;
        imageWidth_ = lastFrame_.width;
        imageCenter_ = cv::Point(lastFrame_.width / 2, lastFrame_.height / 2);
    }

}

void ULineFinder::adjustFrame(cv::Mat image) {
    if (!static_cast<bool> (autoHsvValueRange)) {
        return;
    }
    cv::Scalar mean = cv::mean(image);
    cerr << "Mean: " << mean << ", " << endl;
}

void ULineFinder::processFrame() {
    if (!hasFrame_ || lastProcessedFrameNo_ >= lastReceivedFrameNo_) {
        return;
    }
    lastProcessedFrameNo_ = lastReceivedFrameNo_;
    /*
     * Image Processing
     */
    cv::Mat src(lastFrame_.height, lastFrame_.width, CV_8UC3, lastFrame_.data);
    cv::Mat mid;

    //adjustFrame(src);

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



    /*
     * Output vars
     */
    // set line orientation "to the front"
    if (avg[1] < avg[3]) {
        std::swap(avg[0], avg[2]);
        std::swap(avg[1], avg[3]);
    }
    line = avg;

    // find distance from the middle
    double dev(distance_); // use previous deviation
    double devAging = static_cast<double> (distanceAging);
    cv::Point norm(avg[3] - avg[1], avg[0] - avg[2]);
    cv::Scalar color;
    if (norm.x == 0 && norm.y == 0) {
        color = cv::Scalar(0, 128, 128);
    } else {
        auto c = -norm.dot(cv::Point(avg[0], avg[1]));
        distance_ = dev = (1 - devAging) * dev + devAging * ((norm.dot(imageCenter_) + c) / hypot(norm.x, norm.y)); // weighted average of current and previous deviation
        color = (dev > 0) ? cv::Scalar(0, 255, 255) : cv::Scalar(0, 255, 0);
    }

    /*
     * Output vars
     */
    vector<int> l = vector<int>(4);
    l[0] = avg[0];
    l[1] = avg[1];
    l[2] = avg[2];
    l[3] = avg[3];
    line = l;

    distance = distance_;

    // deviation angle of computed line and frontal axis
    if (norm.x != 0) {
        deviation = atan(norm.y / norm.x);
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
