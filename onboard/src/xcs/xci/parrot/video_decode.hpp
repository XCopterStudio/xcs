#ifndef VIDEO_DECODE_H
#define VIDEO_DECODE_H

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

//#pragma comment (lib, "avcodec.lib")
//#pragma comment (lib, "avformat.lib")

namespace xcs {
namespace xci {
namespace parrot {

class VideoDecoder {
    AVCodec* codec_;
    AVCodecContext * context_;
    AVFrame *frame_;
    uint8_t* bufferFrameOut;
    AVFrame *frameOut_;
    SwsContext* swsContext_;
public:
    VideoDecoder() : codec_(nullptr), context_(nullptr), frame_(nullptr), frameOut_(nullptr), swsContext_(nullptr), bufferFrameOut(nullptr){};
    void init(const AVCodecID avCodec);
    bool decodeVideo(AVPacket* avpacket);
    AVFrame* decodedFrame();
    
    ~VideoDecoder();
};

}
}
}
#endif
