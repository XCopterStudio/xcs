#include "video_decode.hpp"

using namespace xcs;

void VideoDecoder::init(const AVCodecID avCodec) {
    // register all codecs
    av_register_all();

    // find adequate codec 
    codec = avcodec_find_decoder(avCodec);
    if (!codec) { // TODO: throw exception
        fprintf(stderr, "Could not find encoder\n");
        exit(1);
    }

    context = avcodec_alloc_context3(codec);
    if (!context) { // TODO: throw exception
        fprintf(stderr, "Could not open context\n");
        exit(1);
    }

    if (codec->capabilities & CODEC_CAP_TRUNCATED)
        context->flags |= CODEC_FLAG_TRUNCATED; /* we do not send complete frames */
    /* For some codecs, such as msmpeg4 and mpeg4, width and height
       MUST be initialized there because this information is not
       available in the bitstream. */

    /* open it */
    if (avcodec_open2(context, codec, NULL) < 0) { // TODO: throw exception
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }

    frame = avcodec_alloc_frame();
    if (!frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }

}

void VideoDecoder::decodeVideo(const VideoParams* params, AVPacket* avpacket) {
    int len, gotFrame;
    len = avcodec_decode_video2(context, frame, &gotFrame, avpacket);
}