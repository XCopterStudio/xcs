#ifndef VIDEO_DECODE_H
#define VIDEO_DECODE_H

#include <libavcodec\avcodec.h>

#define INBUF_SIZE 4096

class videoDecoder{
	AVCodec* codec;
	AVCodecContext * context;
	AVFrame *frame;
public:
	videoDecoder(const AVCodecID avCodec);
};

#endif