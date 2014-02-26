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

    cv::namedWindow("Display Image", cv::WINDOW_AUTOSIZE);
    //cv::imshow("Display Image", image);

    cv::waitKey(20);
}

void ULineFinder::onChangeVideo(::urbi::UImage &image) {

    cv::Mat img(image.width, image.height, CV_8UC3, image.data);

    cv::imshow("Display Image", img);

    cv::waitKey(20);
}

UStart(ULineFinder);
