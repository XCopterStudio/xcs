#include "u_line_finder.hpp"
#include <opencv2/opencv.hpp>

using namespace xcs::urbi;

ULineFinder::ULineFinder(const std::string &name) :
    ::urbi::UObject(name) {

    UBindFunction(ULineFinder, init);

    UBindVar(ULineFinder, video);
    UNotifyChange(video, &ULineFinder::onChangeVideo);
}

void ULineFinder::init() {
    std::cout << "Initing";
    /*cv::Mat image;
    image = cv::imread("/home/maros/projects/drones/xcs/onboard/src/xcs/urbi/line_finder.uob/sample.jpg",1);
    if (!image.data) {
        std::cout << "NO IMAGE DATA";
    }*/

    cv::namedWindow("Source", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Blured", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("HSV->InRange->Canny", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("EdgeMap", cv::WINDOW_AUTOSIZE);
}


void ULineFinder::onChangeVideo(::urbi::UVar &uvar) {
    ::urbi::UImage image = uvar;
    cv::Mat src(image.height, image.width, CV_8UC3, image.data);
    cv::Mat mid, dst;

    // show original image
    cv::imshow("Source", src);

    // denoise
    cv::GaussianBlur(src, mid, cv::Size(5,5), 0, 0);
    cv::imshow("Blured", mid);

    // convert to HSV
    cv::cvtColor(mid, mid, CV_BGR2HSV);
    // highlight ROI (region of interest)
    // value between 0-40
    cv::inRange(mid, cv::Scalar(0,0,0), cv::Scalar(255,255,40), mid);
    // transform to edge map
    cv::Canny(mid, mid, 200, 200, 3);
    cv::imshow("HSV->InRange->Canny", mid);

    // detect rectangle
    cv::vector<cv::Vec4i> lines;
    cv::HoughLinesP(mid, lines, 1, CV_PI/180, 70, 30, 10);
//    if (lines.size() > 0)
//        cv::line(src, cv::Point(lines[0][0], lines[0][1]), cv::Point(lines[0][2], lines[0][3]), cv::Scalar(255,0,0), 2, CV_AA);
    for (auto l : lines) {
        cv::line(src, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(255,0,0), 2, CV_AA);
    }
    cv::imshow("EdgeMap", src);

    cv::waitKey(20);
}

UStart(ULineFinder);
