#include "netadapter.hpp"
#include <xcs/nodes/xobject/x.h>
#include <xcs/logging.hpp>
#include "video_writer.hpp"

// temporary for video display
//#include <opencv2/opencv.hpp>

namespace xcs {
namespace nodes {

NetAdapter::NetAdapter(const std::string &name) :
  XObject(name),
  video("VIDEO") {

    XBindVarF(video, &NetAdapter::onChangeVideo);
    XBindFunction(NetAdapter, init);
}

NetAdapter::~NetAdapter() {}

void NetAdapter::init(const std::string &location, const std::string &mimetype) {
//    cv::namedWindow("Test", cv::WINDOW_AUTOSIZE);
    videoWriter_ = new VideoWriter(location, mimetype, 640, 480);
}

void NetAdapter::onChangeVideo(::urbi::UImage frame) { // TODO: try to change UVar to UImage.. should work
//    cv::Mat src(frame.height, frame.width, CV_8UC3, frame.data);
//    cv::imshow("Test", src);
//    cv::waitKey(10);
    videoWriter_->write(frame);
}

XStart(NetAdapter);

}
}
