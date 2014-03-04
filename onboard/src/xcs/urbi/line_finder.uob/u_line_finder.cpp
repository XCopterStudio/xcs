#include "u_line_finder.hpp"
#include <opencv2/opencv.hpp>

using namespace xcs::urbi;

ULineFinder::ULineFinder(const std::string &name) :
  ::urbi::UObject(name),
  blurRange_(5),
  hsvValueRange_(120),
  cannyT1_(200),
  cannyT2_(200),
  cannyApertureSize_(3),
  cannyL2Gradient_(false),
  houghRho_(4),
  houghTheta_(CV_PI / 180),
  houghT_(70),
  houghMinLength_(100),
  houghMaxGap_(40),
  line_(4),
  center_(2, 0),
  prevDist_(0) {

    UBindFunction(ULineFinder, init);

    UBindVar(ULineFinder, video);
    UNotifyChange(video, &ULineFinder::onChangeVideo);

    UBindFunction(ULineFinder, getLine);
    UBindFunction(ULineFinder, getViewCenter);

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
    UNotifyChange(blurRange, &ULineFinder::onChangeBlurRange);
    UNotifyChange(hsvValueRange, &ULineFinder::onChangeHsvValueRange);
    UNotifyChange(cannyT1, &ULineFinder::onChangeCannyT1);
    UNotifyChange(cannyT2, &ULineFinder::onChangeCannyT2);
    UNotifyChange(cannyApertureSize, &ULineFinder::onChangeCannyApertureSize);
    UNotifyChange(cannyL2Gradient, &ULineFinder::onChangeCannyL2Gradient);
    UNotifyChange(houghRho, &ULineFinder::onChangeHoughRho);
    UNotifyChange(houghTheta, &ULineFinder::onChangeHoughTheta);
    UNotifyChange(houghT, &ULineFinder::onChangeHoughT);
    UNotifyChange(houghMinLength, &ULineFinder::onChangeHoughMinLength);
    UNotifyChange(houghMaxGap, &ULineFinder::onChangeHoughMaxGap);
}

void ULineFinder::init() {
    std::cout << "Initing";
    /*cv::Mat image;
    image = cv::imread("/home/maros/projects/drones/xcs/onboard/src/xcs/urbi/line_finder.uob/sample.jpg",1);
    if (!image.data) {
        std::cout << "NO IMAGE DATA";
    }*/

    blurRange = blurRange_;
    hsvValueRange = hsvValueRange_;
    cannyT1 = cannyT1_;
    cannyT2 = cannyT2_;
    cannyApertureSize = cannyApertureSize_;
    cannyL2Gradient = cannyL2Gradient_;
    houghRho = houghRho_;
    houghTheta = houghTheta_;
    houghT = houghT_;
    houghMinLength = houghMinLength_;
    houghMaxGap = houghMaxGap_;

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

void ULineFinder::onChangeVideo(::urbi::UVar &uvar) {
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
    cv::GaussianBlur(src, mid, cv::Size(blurRange_, blurRange_), 0, 0);
    cv::imshow("Blured", mid);

    // convert to HSV
    cv::cvtColor(mid, mid, CV_BGR2HSV);
    // highlight ROI (region of interest)
    // value between 0-40
    cv::inRange(mid, cv::Scalar(0, 0, 0), cv::Scalar(255, 255, hsvValueRange_), mid);
    cv::imshow("HSV->InRange", mid);
    // transform to edge map
    cv::Canny(mid, mid, cannyT1_, cannyT2_, cannyApertureSize_, cannyL2Gradient_);
    cv::imshow("Canny", mid);

    // detect rectangle
    cv::vector<cv::Vec4i> lines;
    cv::HoughLinesP(mid, lines, houghRho_, houghTheta_, houghT_, houghMinLength_, houghMaxGap_);
    //    if (lines.size() > 0)
    //        cv::line(src, cv::Point(lines[0][0], lines[0][1]), cv::Point(lines[0][2], lines[0][3]), cv::Scalar(255,0,0), 2, CV_AA);
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

    cv::waitKey(20);
}

void ULineFinder::onChangeBlurRange(int range) {
    blurRange_ = range;
}

void ULineFinder::onChangeHsvValueRange(double range) {
    hsvValueRange_ = range;
}

void ULineFinder::onChangeCannyT1(double treshhold) {
    cannyT1_ = treshhold;
}

void ULineFinder::onChangeCannyT2(double treshhold) {
    cannyT2_ = treshhold;
}

void ULineFinder::onChangeCannyApertureSize(int size) {
    cannyApertureSize_ = size;
}

void ULineFinder::onChangeCannyL2Gradient(bool hasGradient) {
    cannyL2Gradient_ = hasGradient;
}

void ULineFinder::onChangeHoughRho(double rho) {
    houghRho_ = rho;
}

void ULineFinder::onChangeHoughTheta(double theta) {
    houghTheta_ = theta;
}

void ULineFinder::onChangeHoughT(int treshhold) {
    houghT_ = treshhold;
}

void ULineFinder::onChangeHoughMinLength(double length) {
    houghMinLength_ = length;
}

void ULineFinder::onChangeHoughMaxGap(double length) {
    houghMaxGap_ = length;
}

UStart(ULineFinder);
