#ifndef VIDEO_DECODE_H
#define VIDEO_DECODE_H

#include "xcs/xcs_export.h"

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>


//#pragma comment (lib, "avcodec.lib")
//#pragma comment (lib, "avformat.lib")

namespace xcs{

struct XCS_EXPORT VideoParams {
    unsigned int height;
    unsigned int width;
    unsigned int videoCodec;
};

class XCS_EXPORT VideoDecoder {
    AVCodec* codec;
    AVCodecContext * context;
    AVFrame *frame;
public:
    void init(const AVCodecID avCodec);
    void decodeVideo(const VideoParams* params, AVPacket* avpacket);
};

}
#endif