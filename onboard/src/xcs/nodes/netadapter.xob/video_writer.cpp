#include "video_writer.hpp"

using namespace std;
using namespace std::chrono;
using namespace xcs::nodes::datalogger;

VideoWriter::VideoWriter(const std::string &name) :
  AbstractWriter(name),
  frameNumber_(0) {
    avframe_ = avcodec_alloc_frame();
}

VideoWriter::~VideoWriter() {
    avcodec_free_frame(&avframe_);
}

void VideoWriter::init(const std::string &videoFile, const unsigned int &width, const unsigned int &height, const std::string &dataName, LoggerContext &context, ::urbi::UVar &uvar) {
    basicInit(dataName, context, uvar);
    videoFileWriter_ = unique_ptr<VideoFileWriter>(new VideoFileWriter(videoFile, width, height));
    //UNotifyThreadedChange(uvar, &VideoWriter::write, ::urbi::LOCK_FUNCTION);
    UNotifyChange(uvar, &VideoWriter::write);
}

void VideoWriter::write(urbi::UImage image) {
    if (!context_->enabled) {
        return;
    }

    {
        std::lock_guard<std::mutex> lck(context_->lock);
        writeRecordBegin();

        context_->file << frameNumber_++;
        context_->file << endl;
    }

    avframe_->width = image.width;
    avframe_->height = image.height;
    avframe_->format = PIX_FMT_BGR24;
    //cerr << "Video image [" << image.width << "," << image.height << "]" << endl;
    avpicture_fill((AVPicture*) avframe_, image.data, (AVPixelFormat) avframe_->format, image.width, image.height);
    videoFileWriter_->writeVideoFrame(*avframe_);
}
