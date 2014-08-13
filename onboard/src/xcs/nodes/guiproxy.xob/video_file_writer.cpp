#include "video_file_writer.hpp"
#include <iostream>
#include <xcs/logging.hpp>

extern "C" {
#include "libavutil/mathematics.h"
}

#ifdef _WIN32
#define snprintf _snprintf_s
#endif

using namespace xcs::nodes;

const unsigned int VideoFileWriter::STREAM_FRAME_RATE = 25;
const unsigned int VideoFileWriter::VIDEO_BUFFER_SIZE = 200000;
const AVPixelFormat VideoFileWriter::STREAM_PIX_FMT = PIX_FMT_YUV420P;

void VideoFileWriter::prepareCodecContext(
        AVCodecContext *codecContext,
        const CodecID codecId,
        const unsigned int &width,
        const unsigned int &height,
        const unsigned int &bitrate,
        bool leaveDefault) {

    avcodec_get_context_defaults3(codecContext, codec_);

    // set necessary context
    codecContext->codec_id = codecId;
    codecContext->codec_type = AVMEDIA_TYPE_VIDEO;
    codecContext->width = width;
    codecContext->height = height;
    codecContext->time_base.den = STREAM_FRAME_RATE;
    codecContext->time_base.num = 1;
    codecContext->pix_fmt = STREAM_PIX_FMT;

    if (!leaveDefault) {
        codecContext->bit_rate = bitrate;
    }

    // codec-specific settings
    switch(codecId) {
        case AV_CODEC_ID_VP8:
            codecContext->gop_size = 1; // number of pictures in a group of pictures
                                        // (emit one intra frame for each gop_size number frames at most)
            break;
        case AV_CODEC_ID_H264:
            codecContext->bit_rate = 500*10;//500*1000;
            codecContext->bit_rate_tolerance = 0;
            codecContext->rc_max_rate = 0;
            codecContext->rc_buffer_size = 0;
            codecContext->gop_size = 40;
            codecContext->max_b_frames = 3;
            codecContext->b_frame_strategy = 1;
            codecContext->coder_type = 1;
            codecContext->me_cmp = 1;
            codecContext->me_range = 16;
            codecContext->qmin = 10;
            codecContext->qmax = 51;
            codecContext->scenechange_threshold = 40;
            codecContext->flags |= CODEC_FLAG_LOOP_FILTER;
            codecContext->me_method = ME_HEX;
            codecContext->me_subpel_quality = 5;
            codecContext->i_quant_factor = 0.71;
            codecContext->qcompress = 0.6;
            codecContext->max_qdiff = 4;
            //codecContext->directpred = 1;
            //codecContext->flags2 |= CODEC_FLAG2_FASTPSKIP;
            break;
        case AV_CODEC_ID_MPEG2VIDEO:
            /* just for testing, we also add B frames */
            codecContext->max_b_frames = 2;
            break;
        case AV_CODEC_ID_MPEG1VIDEO:
            /* Needed to avoid using macroblocks in which some coeffs overflow.
            This does not happen with normal video, it just happens here as
            the motion of the chroma plane does not match the luma plane. */
            codecContext->mb_decision = 2;
            break;
        default:
            break;
    }

    // some formats want stream headers to be separate
    if (formatContext_->oformat->flags & AVFMT_GLOBALHEADER)
        codecContext->flags |= CODEC_FLAG_GLOBAL_HEADER;

}

AVFrame* VideoFileWriter::allocPicture(const PixelFormat pixelFormat, const int width, const int height) {
    AVFrame *picture;
    uint8_t *pictureBuffer;
    int size;

    picture = avcodec_alloc_frame();
    if (!picture){
        XCS_LOG_ERROR("Could not allocate avframe.");
        return nullptr;
    }

    size = avpicture_get_size(pixelFormat, width, height);
    pictureBuffer = (uint8_t*) av_malloc(size);

    if (!pictureBuffer){
        XCS_LOG_ERROR("Could not allocate picture buffer.");
        av_free(picture);
        return nullptr;
    }

    avpicture_fill((AVPicture*)picture, pictureBuffer, pixelFormat, width, height);
    return picture;
}

VideoFileWriter::VideoFileWriter(const std::string &url, const std::string &mimetype, const unsigned int &width, const unsigned int &height, const unsigned int &bitrate) :
  url_(url),
  mimetype_(mimetype),
  width_(width),
  height_(height),
  bitrate_(bitrate),
  outputFormat_(nullptr),
  formatContext_(nullptr),
  codec_(nullptr),
  videoStream_(nullptr),
  picture_(nullptr),
  pictureConvertContext_(nullptr),
  videoBuffer_(nullptr),
  isWriteable_(false) {

    av_register_all();
    avformat_network_init();
}

VideoFileWriter::~VideoFileWriter() {
    closeVideo();
    avformat_network_deinit();
}

void VideoFileWriter::openVideo() {
    initMtx_.lock();

    pictureConvertContext_ = nullptr;

    /// Determine output format.
    outputFormat_ = av_guess_format(NULL, NULL, mimetype_.c_str());
    if (!outputFormat_) {
        XCS_LOG_ERROR("Could not deduce output format from MIME type: trying deduction from filename.");
        outputFormat_ = av_guess_format(NULL, url_.c_str(), NULL);
    }
    if (!outputFormat_) {
        XCS_LOG_ERROR("Could not deduce output format from file extension: using mpeg.");
        outputFormat_ = av_guess_format("mpeg", NULL, NULL);
    }
    if (!outputFormat_) {
        XCS_LOG_ERROR("Coult not find suitable output format.");
        return;
    }

    /* allocate the output media context */
    formatContext_ = avformat_alloc_context();
    if (!formatContext_) {
        XCS_LOG_ERROR("Could not allocate memory for the avformat context.");
        return;
    }

    formatContext_->oformat = outputFormat_;
    snprintf(formatContext_->filename, sizeof(formatContext_->filename), "%s", url_.c_str());

    /// Create video stream.
    videoStream_ = nullptr;
    videoStream_ = avformat_new_stream(formatContext_, NULL);
    if (!videoStream_) {
        XCS_LOG_ERROR("Could not alloc video stream.");
        return;
    }

    /// Get video encoder.
    AVCodecID codecId = outputFormat_->video_codec;
    codec_ = avcodec_find_encoder(codecId);
    if (!codec_) {
        XCS_LOG_ERROR("Could not find encoder.");
    }

    /// Prepare codec context.
    AVCodecContext *codecContext;
    codecContext = videoStream_->codec;
    prepareCodecContext(codecContext, codecId, width_, height_, bitrate_, true);

    if (videoStream_) {
        /// Open codec.
        if (avcodec_open2(codecContext, codec_, NULL)){
            XCS_LOG_ERROR("Could not open codec.");
        }
        /// Allocate video buffer.
        videoBuffer_ = nullptr;
        if (!(formatContext_->oformat->flags & AVFMT_RAWPICTURE)) {
            /* allocate output buffer */
            /* XXX: API change will be done */
            /* buffers passed into lav* can be allocated any way you prefer,
            as long as they're aligned enough for the architecture, and
            they're freed appropriately (such as using av_free for buffers
            allocated with av_malloc) */
            videoBuffer_ = (uint8_t*) av_malloc(VIDEO_BUFFER_SIZE);
        }

        /// Allocate the encoded raw picture.
        picture_ = allocPicture(codecContext->pix_fmt, codecContext->width, codecContext->height);
        if (!picture_){
            XCS_LOG_ERROR("Could not allocate picture.");
            return;
        }
    }

    /// Open the output location, if needed.
    if (!(outputFormat_->flags & AVFMT_NOFILE)) {
        if (avio_open(&formatContext_->pb, url_.c_str(), AVIO_FLAG_WRITE) < 0) {
            XCS_LOG_ERROR("Could not open " + url_);
            return ;
        }
    }

    /// Write format header.
    avformat_write_header(formatContext_, NULL);

    isWriteable_ = true;

    av_dump_format(formatContext_, 0, url_.c_str(), 1);

    initMtx_.unlock();
}

void VideoFileWriter::closeVideo() {
    initMtx_.lock();

    isWriteable_ = false;

    av_write_trailer(formatContext_);
    if (videoStream_){
        avcodec_close(videoStream_->codec);
        av_free(picture_->data[0]);
        av_free(picture_);
        av_free(videoBuffer_);
    }
    for (int i = 0; i < formatContext_->nb_streams; ++i){
        av_freep(&formatContext_->streams[i]->codec);
        av_freep(&formatContext_->streams[i]);
    }
    if (!(outputFormat_->flags & AVFMT_NOFILE)){
        avio_close(formatContext_->pb);
    }
    av_free(formatContext_);

    initMtx_.unlock();
}

void VideoFileWriter::writeVideoFrame(const AVFrame& frame){

    if (!isWriteable_) {
        return;
    }

    //std::cerr << ".";

    AVCodecContext *codecContext;
    codecContext = videoStream_->codec;

    //std::cerr << "Video [" << frame.width << "," << frame.height << "]" << " out [" << codecContext->width << "," << codecContext->height << "]" << std::endl;

    if (pictureConvertContext_ == nullptr){
        pictureConvertContext_ = sws_getCachedContext(pictureConvertContext_,
            frame.width,
            frame.height,
            (AVPixelFormat)frame.format,
            codecContext->width,
            codecContext->height,
            codecContext->pix_fmt,
            SWS_BICUBIC,
            NULL,
            NULL,
            NULL);
        if (pictureConvertContext_ == nullptr){
            XCS_LOG_ERROR("Cannot initialize the conversion context.");
            return;
        }
    }

    sws_scale(pictureConvertContext_, frame.data, frame.linesize, 0, codecContext->height, picture_->data, picture_->linesize);

    int returnSize, outSize;
    if (formatContext_->oformat->flags & AVFMT_RAWPICTURE) {
        /* raw video case. The API will change slightly in the near
        futur for that */
        AVPacket packet;
        av_init_packet(&packet);

        packet.flags |= AV_PKT_FLAG_KEY;
        packet.stream_index = videoStream_->index;
        packet.data = (uint8_t *)picture_;
        packet.size = sizeof(AVPicture);

        returnSize = av_interleaved_write_frame(formatContext_, &packet);
    }
    else {
        /* encode the image */
        outSize = avcodec_encode_video(codecContext, videoBuffer_, VIDEO_BUFFER_SIZE, picture_);
        /* if zero size, it means the image was buffered */
        if (outSize > 0) {
            AVPacket packet;
            av_init_packet(&packet);

            if (codecContext->coded_frame->pts != AV_NOPTS_VALUE)
                packet.pts = av_rescale_q(codecContext->coded_frame->pts, codecContext->time_base, videoStream_->time_base);
            if (codecContext->coded_frame->key_frame)
                packet.flags |= AV_PKT_FLAG_KEY;
            packet.stream_index = videoStream_->index;
            packet.data = videoBuffer_;
            packet.size = outSize;

            /* write the compressed frame in the media file */
            returnSize = av_interleaved_write_frame(formatContext_, &packet);
        }
        else {
            returnSize = 0;
        }
    }
    if (returnSize != 0) {
        XCS_LOG_ERROR("Error while writing video frame.");
    }
}
