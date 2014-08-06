#include "video_writer.hpp"

using namespace xcs::nodes;

VideoWriter::VideoWriter(const std::string &videoFile, const unsigned int &width, const unsigned int &height) : //AbstractWriter(name),
  frameNumber_(0) {

    avframe_ = avcodec_alloc_frame();

    avformat_network_init();

    videoFileWriter_ = new VideoFileWriter(videoFile, width, height);
}

VideoWriter::~VideoWriter() {
    avcodec_free_frame(&avframe_);
    delete videoFileWriter_;
}

void VideoWriter::write(urbi::UImage image) {

    avframe_->width = image.width;
    avframe_->height = image.height;
    avframe_->format = PIX_FMT_BGR24;
    //cerr << "Video image [" << image.width << "," << image.height << "]" << endl;
    avpicture_fill((AVPicture*) avframe_, image.data, (AVPixelFormat) avframe_->format, image.width, image.height);
    videoFileWriter_->writeVideoFrame(*avframe_);
}
