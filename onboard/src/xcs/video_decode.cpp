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

    //set codec context for h264 and parrot
    if (AV_CODEC_ID_H264){
        context->pix_fmt = PIX_FMT_YUV420P;
        context->skip_frame = AVDISCARD_DEFAULT;
        context->error_concealment = FF_EC_GUESS_MVS | FF_EC_DEBLOCK;
        //context->err_recognition = AV_EF_CRCCHECK;
        context->skip_loop_filter = AVDISCARD_DEFAULT;
        context->workaround_bugs = FF_BUG_AUTODETECT;
        context->codec_type = AVMEDIA_TYPE_VIDEO;
        context->codec_id = CODEC_ID_H264;
        context->skip_idct = AVDISCARD_DEFAULT;
    }

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

bool VideoDecoder::decodeVideo(AVPacket* avpacket) {
    int len, gotFrame;
    len = avcodec_decode_video2(context, frame, &gotFrame, avpacket);
    return gotFrame;
}

AVFrame* VideoDecoder::decodedFrame(){
    return frame;
}