#include "u_line_finder.hpp"
#include <opencv2/opencv.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>


using namespace xcs::urbi;

ULineFinder::ULineFinder(const std::string &name) :
  ::urbi::UObject(name),
  line_(4),
  center_(2, 0),
  prevDist_(0) {

    UBindFunction(ULineFinder, init);

    UBindVar(ULineFinder, video);
    UNotifyChange(video, &ULineFinder::onChangeVideo);

    UBindFunction(ULineFinder, getLine);
    UBindFunction(ULineFinder, getViewCenter);
    UBindFunction(ULineFinder, getImageWidth);
    UBindFunction(ULineFinder, getImageHeight);

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

    cv::namedWindow("Source", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Blured", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("HSV->InRange", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Canny", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("EdgeMap", cv::WINDOW_AUTOSIZE);
}

std::vector<int> ULineFinder::getLine() {
    return line_;
}

std::vector<int> ULineFinder::getViewCenter() {
    return center_;
}

int ULineFinder::getImageWidth() {
    return imageWidth_;
}

int ULineFinder::getImageHeight() {
    return imageHeight_;
}

void ULineFinder::onChangeVideo(::urbi::UVar &uvar) {
    BOOST_LOG_TRIVIAL(trace) << "onChange entry";
    ::urbi::UImage image = uvar;
    // set view center
    if ((imageHeight_ != image.height) || (imageWidth_ != image.width)) {
        imageHeight_ = image.height;
        imageWidth_ = image.width;
        center_[0] = imageWidth_ / 2;
        center_[1] = imageHeight_ / 2;
    }

    /*
     * Image Processing
     */
    cv::Mat src(image.height, image.width, CV_8UC3, image.data);
    cv::Mat mid, dst;

    // show original image
    cv::imshow("Source", src);

    // denoise
    cv::GaussianBlur(src, mid, cv::Size(static_cast<int> (blurRange), static_cast<int> (blurRange)), 0, 0);
    cv::imshow("Blured", mid);

    // convert to HSV
    cv::cvtColor(mid, mid, CV_BGR2HSV);
    // highlight ROI (region of interest)
    // value between 0-40
    cv::inRange(mid, cv::Scalar(0, 0, 0), cv::Scalar(255, 255, static_cast<double> (hsvValueRange)), mid);
    cv::imshow("HSV->InRange", mid);
    // transform to edge map
    cv::Canny(mid, mid, static_cast<double> (cannyT1), static_cast<double> (cannyT2), static_cast<int> (cannyApertureSize), static_cast<double> (cannyL2Gradient));
    cv::imshow("Canny", mid);

    // detect rectangle
    cv::vector<cv::Vec4i> lines;
    cv::HoughLinesP(mid, lines, static_cast<double> (houghRho), static_cast<double> (houghTheta), static_cast<int> (houghT), static_cast<double> (houghMinLength), static_cast<double> (houghMaxGap));

    for (auto l : lines) {
        cv::line(src, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(255, 0, 0), 2, CV_AA);
    }
    // compute single vector from lines
    cv::Vec4i avg = cv::mean(lines);


    // find distance from the middle
    cv::Point middle(image.width / 2, image.height / 2);
    double dist(prevDist_); // use previous dist
    cv::Point norm(avg[3] - avg[1], avg[0] - avg[2]);
    cv::Scalar color;
    if (norm.x == 0 && norm.y == 0) {
        color = cv::Scalar(0, 128, 128);
    } else {
        color = cv::Scalar(0, 255, 255);
        auto c = -norm.dot(cv::Point(avg[0], avg[1]));
        prevDist_ = dist = 0.2 * dist + 0.8 * (abs(norm.dot(middle) + c) / hypot(norm.x, norm.y)); // weighted average of current and previous dist
    }


    cv::circle(src, middle, dist, color, 3, CV_AA);
    cv::line(src, cv::Point(avg[0], avg[1]), cv::Point(avg[2], avg[3]), cv::Scalar(0, 0, 255), 3, CV_AA);
    cv::circle(src, cv::Point(avg[2], avg[3]), 5, cv::Scalar(0, 0, 255), 3, CV_AA);
    cv::imshow("EdgeMap", src);

    line_[0] = avg[0];
    line_[1] = avg[1];
    line_[2] = avg[2];
    line_[3] = avg[3];
    BOOST_LOG_TRIVIAL(trace) << "onChange pre-exit";

    cv::waitKey(1);
    BOOST_LOG_TRIVIAL(trace) << "onChange exit";
}

UStart(ULineFinder);
