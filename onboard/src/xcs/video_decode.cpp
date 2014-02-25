#include "video_decode.hpp"

using namespace xcs;

void VideoDecoder::init(const AVCodecID avCodec) {
    // register all codecs
    av_register_all();

    // find adequate codec 
    codec_ = avcodec_find_decoder(avCodec);
    if (!codec_) { // TODO: throw exception
        fprintf(stderr, "Could not find encoder\n");
        exit(1);
    }

    context_ = avcodec_alloc_context3(codec_);
    if (!context_) { // TODO: throw exception
        fprintf(stderr, "Could not open context\n");
        exit(1);
    }

    if (codec_->capabilities & CODEC_CAP_TRUNCATED)
        context_->flags |= CODEC_FLAG_TRUNCATED; /* we do not send complete frames */
    /* For some codecs, such as msmpeg4 and mpeg4, width and height
       MUST be initialized there because this information is not
       available in the bitstream. */

    //set codec context for h264 and parrot
    if (AV_CODEC_ID_H264){
        context_->pix_fmt = PIX_FMT_YUV420P;
        context_->skip_frame = AVDISCARD_DEFAULT;
        context_->error_concealment = FF_EC_GUESS_MVS | FF_EC_DEBLOCK;
        //context->err_recognition = AV_EF_CRCCHECK;
        context_->skip_loop_filter = AVDISCARD_DEFAULT;
        context_->workaround_bugs = FF_BUG_AUTODETECT;
        context_->codec_type = AVMEDIA_TYPE_VIDEO;
        context_->codec_id = CODEC_ID_H264;
        context_->skip_idct = AVDISCARD_DEFAULT;
    }

    /* open it */
    if (avcodec_open2(context_, codec_, NULL) < 0) { // TODO: throw exception
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }

    frame_ = avcodec_alloc_frame();
    if (!frame_) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }

}

bool VideoDecoder::decodeVideo(AVPacket* avpacket) {
    int len, gotFrame;
    len = avcodec_decode_video2(context_, frame_, &gotFrame, avpacket);
    return gotFrame;
}

AVPicture* VideoDecoder::decodedPicture(){
    if(picture_ == nullptr || frame_->height != videoParams_.height || frame_->width != videoParams_.width){
        videoParams_.height = frame_->height;
        videoParams_.width = frame_->width;
        videoParams_.videoCodec = frame_->format;
        //unsigned int = avpicture_get_size(PIX_FMT_BGR24,)

            /*out->size = avpicture_get_size(cfg->dst_picture.format, cfg->dst_picture.width, cfg->dst_picture.height);
        cfg->buffer = (uint8_t *)av_realloc(cfg->buffer, out->size * sizeof(uint8_t));
        out->buffers[0] = cfg->buffer;

        avpicture_fill((AVPicture *)pFrameOutput, (uint8_t*)out->buffers[out->indexBuffer], cfg->dst_picture.format,
            cfg->dst_picture.width, cfg->dst_picture.height);


        cfg->img_convert_ctx = sws_getCachedContext(cfg->img_convert_ctx, PaVE.display_width, PaVE.display_height,
            pCodecCtxH264->pix_fmt, PaVE.display_width, PaVE.display_height,
            cfg->dst_picture.format, sws_flags, NULL, NULL, NULL);*/
    }

    return picture_;
}

VideoDecoder::~VideoDecoder(){
    avcodec_free_frame(&frame_);
}