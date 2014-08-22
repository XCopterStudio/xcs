#ifndef VIDEO_FILE_WRITER_H
#define VIDEO_FILE_WRITER_H

#include <string>
#include <mutex>

extern "C" {
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

namespace xcs{
namespace nodes{

class VideoFileWriter{

    std::string url_;
    std::string mimetype_;
    int width_;
    int height_;
    int bitrate_;

    static const unsigned int STREAM_FRAME_RATE;
    static const unsigned int VIDEO_BUFFER_SIZE;
    static const AVPixelFormat STREAM_PIX_FMT;

    AVOutputFormat *outputFormat_;
    AVFormatContext *formatContext_;
    AVCodec *codec_;
    AVStream *videoStream_;
    AVFrame *picture_;
    SwsContext *pictureConvertContext_;

    uint8_t *videoBuffer_;
    std::mutex initMtx_;
    bool isWriteable_;
   
    void prepareCodecContext(AVCodecContext *codecContext, const CodecID codecId, const unsigned int &width, const unsigned int &height, const unsigned int &bitrate, bool leaveDefault = true);
    AVFrame* allocPicture(const PixelFormat pixelFormat, const int width, const int height);

public:
    VideoFileWriter(const std::string &fileName, const std::string &mimetype, const unsigned int &width = 640, const unsigned int &height = 480, const unsigned int &bitrate = 400000);
    ~VideoFileWriter();
    void openVideo();
    void freeResources();
    void writeVideoFrame(const AVFrame& frame);
};

}}

#endif
