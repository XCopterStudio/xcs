#ifndef VIDEO_DECODE_H
#define VIDEO_DECODE_H

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

//#pragma comment (lib, "avcodec.lib")
//#pragma comment (lib, "avformat.lib")

namespace xcs {
namespace xci {
namespace parrot {

struct VideoParams {
    unsigned int height;
    unsigned int width;
    unsigned int videoCodec;
    VideoParams(unsigned int height = 0, unsigned int width = 0, unsigned int videoCodec = 0) : height(height), width(width), videoCodec(videoCodec) {};
};

class XCS_EXPORT VideoDecoder {
    AVCodec* codec_;
    AVCodecContext * context_;
    AVFrame *frame_;
    AVPicture* picture_;
    VideoParams videoParams_;
public:
    VideoDecoder() : codec_(nullptr), context_(nullptr), frame_(nullptr), picture_(nullptr) {};
    void init(const AVCodecID avCodec);
    bool decodeVideo(AVPacket* avpacket);
    AVPicture* decodedPicture();
    
    ~VideoDecoder();
};

}
}
}
#endif
