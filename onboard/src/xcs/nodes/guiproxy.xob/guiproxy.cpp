#include "guiproxy.hpp"
#include <xcs/nodes/xobject/x.h>
#include <xcs/logging.hpp>
#include "video_file_writer.hpp"

// uncomment for video display test
//#include <opencv2/opencv.hpp>

namespace xcs {
namespace nodes {

GuiProxy::GuiProxy(const std::string &name) :
  XObject(name),
  video("VIDEO") {

    XBindVarF(video, &GuiProxy::onChangeVideo);
    XBindFunction(GuiProxy, init);
}

GuiProxy::~GuiProxy() {
    avcodec_free_frame(&avframe_);
    delete videoFileWriter_;
}

void GuiProxy::init(const std::string &location, const std::string &mimetype) {
//    cv::namedWindow("Test", cv::WINDOW_AUTOSIZE);
    avframe_ = avcodec_alloc_frame();
    avformat_network_init();
    videoFileWriter_ = new VideoFileWriter(location, mimetype);
}

void GuiProxy::onChangeVideo(::urbi::UImage image) { // TODO: try to change UVar to UImage.. should work
//    cv::Mat src(frame.height, frame.width, CV_8UC3, frame.data);
//    cv::imshow("Test", src);
//    cv::waitKey(10);
    avframe_->width = image.width;
    avframe_->height = image.height;
    avframe_->format = PIX_FMT_BGR24;
    avpicture_fill((AVPicture*) avframe_, image.data, (AVPixelFormat) avframe_->format, image.width, image.height);
    videoFileWriter_->writeVideoFrame(*avframe_);
}

XStart(GuiProxy);

}
}
