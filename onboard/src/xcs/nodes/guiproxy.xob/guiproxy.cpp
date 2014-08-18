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
  video("CAMERA") {

    //XBindVarF(video, &GuiProxy::onChangeVideo);
    XBindVar(video);
    UNotifyThreadedChange(video.data(), &GuiProxy::onChangeVideo, urbi::LOCK_FUNCTION_DROP);
    XBindFunction(GuiProxy, init);
    XBindFunction(GuiProxy, initVideo);
    XBindFunction(GuiProxy, deinitVideo);
}

GuiProxy::~GuiProxy() {
    avcodec_free_frame(&avframe_);
    delete videoFileWriter_;
}

void GuiProxy::init(const std::string &location, const std::string &mimetype) {
//    cv::namedWindow("Test", cv::WINDOW_AUTOSIZE);
    avframe_ = avcodec_alloc_frame();
    videoFileWriter_ = new VideoFileWriter(location, mimetype);
}

void GuiProxy::initVideo() {
    XCS_LOG_INFO("Initializing video transmitter.");
    videoFileWriter_->openVideo();
}

void GuiProxy::deinitVideo() {
    XCS_LOG_INFO("Deinitialize video transmitter.");
    videoFileWriter_->closeVideo();
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
