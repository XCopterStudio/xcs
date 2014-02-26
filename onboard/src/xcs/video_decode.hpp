#ifndef VIDEO_DECODE_H
#define VIDEO_DECODE_H

#include "xcs/xcs_export.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale\swscale.h>
}

//#pragma comment (lib, "avcodec.lib")
//#pragma comment (lib, "avformat.lib")

namespace xcs{

class XCS_EXPORT VideoDecoder {
    AVCodec* codec_;
    AVCodecContext * context_;
    AVFrame *frame_;
    AVFrame *frameOut_;
    SwsContext* swsContext_;
public:
    VideoDecoder() : codec_(nullptr), context_(nullptr), frame_(nullptr), frameOut_(nullptr), swsContext_(nullptr) {};
    void init(const AVCodecID avCodec);
    bool decodeVideo(AVPacket* avpacket);
    AVFrame* decodedFrame();
    
    ~VideoDecoder();
};

}
#endif