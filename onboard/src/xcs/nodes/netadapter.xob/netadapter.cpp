#include "netadapter.hpp"

#include <xcs/nodes/xobject/x.h>
#include <xcs/logging.hpp>

// WebRTC
//#include "webrtc_facade/webrtc_facade.hpp"

// temporary for video display
#include <opencv2/opencv.hpp>

#include <libavformat/avformat.h>

#include "video_writer.hpp"

namespace xcs {
namespace nodes {

NetAdapter::NetAdapter(const std::string &name) :
  XObject(name),
  video("VIDEO") {

    XBindVarF(video, &NetAdapter::onChangeVideo);
    XBindFunction(NetAdapter, init);


//    avframe_ = avcodec_alloc_frame();
//    formatContext_ = avformat_alloc_context();
//    //formatContext_->oformat...


//    stream_ = avformat_new_stream(formatContext_, NULL);
//    codecContext_ = stream_->codec;
//    codecContext_->codec_id = AV_CODEC_ID_MPEG2VIDEO;
//    codecContext_->codec_type = AVMEDIA_TYPE_VIDEO;
//    codecContext_->width = 640;
//    codecContext_->height = 480;
//    codecContext_->time_base = (AVRational) {1,25};
//    codecContext_->pix_fmt = AV_PIX_FMT_YUV420P; // somewhere used without 'AV_' prefix?

//    AVCodec* codec = avcodec_find_encoder(codecContext_->codec_id);
//    if (!codec) {
//        XCS_LOG_ERROR("Codec not found.");
//    }
//    if (avcodec_open2(codecContext_, codec, NULL) < 0) {
//        XCS_LOG_ERROR("Could not open codecl");
//    }



}

NetAdapter::~NetAdapter() {
    //avcodec_free_frame(&avframe_);
}

void NetAdapter::init(const std::string &location, const std::string &mimetype) {
    //webrtcFacade_ = new WebrtcFacade(); // TODO: leak

    cv::namedWindow("Test", cv::WINDOW_AUTOSIZE);

    //av_register_all();

    videoWriter_ = new VideoWriter(location, mimetype, 640, 480);

}

void NetAdapter::onChangeVideo(::urbi::UImage frame) { // TODO: try to change UVar to UImage.. should work
    //::urbi::UImage image = frame;
    //webrtcFacade_->setImageData(image.data, image.size, image.width, image.height);

    //XCS_LOG_INFO("...incomming frame");

//    cv::Mat src(frame.height, frame.width, CV_8UC3, frame.data);
//    cv::imshow("Test", src);
//    cv::waitKey(10);


//    avframe_->width = frame.width;
//    avframe_->height = frame.height;
//    avframe_->format = PIX_FMT_BGR24;
//    //cerr << "Video image [" << image.width << "," << image.height << "]" << endl;
//    avpicture_fill((AVPicture*) avframe_, frame.data, (AVPixelFormat) avframe_->format, frame.width, frame.height);
//    //videoFileWriter_->writeVideoFrame(*avframe_);


//    av_init_packet(&packet_);
//    //if (avcodec_encode_video())
//    //avformat_network_init()

    videoWriter_->write(frame);

}

XStart(NetAdapter);

}
}
