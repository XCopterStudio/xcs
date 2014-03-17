/* 
 * File:   VideoPlayer.cpp
 * Author: michal
 * \see http://blog.tomaka17.com/2012/03/libavcodeclibavformat-tutorial/
 * better \see http://www.sebastianroll.de/TableTopDoc/video_texture_8cpp-source.html#l00302
 * for filters \see http://www.ffmpeg.org/doxygen/trunk/doc_2examples_2filtering_video_8c-example.html
 * 
 * Created on March 3, 2014, 11:47 AM
 */

#include "video_player.hpp"

#include <memory>
#include <vector>
#include <cstdint>
#include <thread>
#include <iostream>
#include <cassert>

#ifdef _WIN32
#define snprintf _snprintf_s
#endif

using namespace std;
using namespace xcs::nodes;
using namespace xcs::xci::dodo;

VideoPlayer::VideoPlayer() : swsContext_(nullptr), hasPic_(false) {

}

void VideoPlayer::init(const string &filename, const string &fontFile) {
    // register all codecs
    av_register_all();

    // prepare context
    avFormat_ = AVFormatContextPtr(avformat_alloc_context(), &avformat_free_context);

    // open file
    auto avFormatPtr = avFormat_.get();
    if (avformat_open_input(&avFormatPtr, filename.c_str(), nullptr, nullptr) != 0) {
        throw runtime_error("Error while calling avformat_open_input (probably invalid file format, filename '" + filename + "')");
    }

    if (avformat_find_stream_info(avFormat_.get(), nullptr) < 0) {
        throw runtime_error("Error while calling avformat_find_stream_info");
    }

    // find stream
    videoStream_ = nullptr;
    for (auto i = 0; i < avFormat_->nb_streams; ++i) {
        if (avFormat_->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            // we've found a video stream!
            videoStream_ = avFormat_->streams[i];
            videoStreamIndex_ = i;
            break;
        }
    }

    if (!videoStream_) {
        throw runtime_error("Didn't find any video stream in the file (probably audio file)");
    }

    // find codec
    // getting the required codec structure
    const auto codec = avcodec_find_decoder(videoStream_->codec->codec_id);
    if (codec == nullptr) {
        throw runtime_error("Codec required by video file not available");
    }

    // allocating a structure
    avVideoCodec_ = AVCodecContextPtr(avcodec_alloc_context3(codec), [](AVCodecContext * c) {
        avcodec_close(c); av_free(c);
    });

    // we need to make a copy of videoStream_->codec->extradata and give it to the context
    // make sure that this vector exists as long as the avVideoCodec exists
    vector<uint8_t> codecContextExtraData(videoStream_->codec->extradata, videoStream_->codec->extradata + videoStream_->codec->extradata_size);
    avVideoCodec_->extradata = codecContextExtraData.data();
    avVideoCodec_->extradata_size = codecContextExtraData.size();

    // initializing the structure by opening the codec
    if (avcodec_open2(avVideoCodec_.get(), codec, nullptr) < 0) {
        throw runtime_error("Could not open codec");
    }

    // prepare for decoding
    avFrame_ = AVFramePtr(avcodec_alloc_frame(), &av_free);

    initFilters("drawtext=fontcolor=white:fontfile=" + fontFile + ":text='%T'");
}

void VideoPlayer::rewind() {
    av_seek_frame(avFormat_.get(), videoStreamIndex_, 0, 0);
}

xcs::nodes::BitmapType VideoPlayer::getFrame() {
    Packet packet_;
    size_t failCounter = 0;

    do {
        while (1) {
            packet_.reset(avFormat_.get());
            if (packet_.packet.stream_index == videoStream_->index) {
                break;
            }
        }

        // sending data to libavcodec
        int isFrameAvailable = 0;
        const auto processedLength = avcodec_decode_video2(avVideoCodec_.get(), avFrame_.get(), &isFrameAvailable, &packet_.packet);
        if (processedLength < 0) {
            throw runtime_error("Error while processing the data");
        }

        // processing the image if available
        if (isFrameAvailable) {
            if (timestamps_) {
                /* push the decoded frame into the filtergraph */
                av_buffersrc_write_frame(bufferSrc_.get(), avFrame_.get());

                /* pull filtered frames from the filtergraph */
                while (1) {
                    AVFilterBufferRef *picref;
                    auto ret = av_buffersink_read(bufferSink_.get(), &picref);

                    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                        break;
                    }
                    if (ret < 0) {
                        break;
                    }
                    avfilter_copy_buf_props(avFrame_.get(), picref);
                    avFrame_.get()->opaque = picref;
                    avfilter_unref_bufferp(&picref);
                }
            }



            // process image
            if (!hasPic_) {
                avpicture_alloc(&pic_, PIX_FMT_RGB24, avFrame_->width, avFrame_->height);
                hasPic_ = true;
            }

            // convert from YUV to RGB
            swsContext_ = sws_getCachedContext(swsContext_, avFrame_->width, avFrame_->height, static_cast<PixelFormat> (avFrame_->format), avFrame_->width, avFrame_->height, PIX_FMT_RGB24, SWS_BILINEAR, nullptr, nullptr, nullptr);

            if (swsContext_ == nullptr) {
                throw runtime_error("Error while calling sws_getContext");
            }
            sws_scale(swsContext_, avFrame_->data, avFrame_->linesize, 0, avFrame_->height, pic_.data, pic_.linesize);

            // format result
            return BitmapType(avFrame_->width, avFrame_->height, pic_.data[0]);
        }
        failCounter++;

        //rewind video
        if (failCounter > 10) { // workaround for corrupted frames in stream, magic value
            rewind();
            failCounter = 0;
        }
    } while (1);

    assert(false); // we should never get here
    return BitmapType();
}

size_t VideoPlayer::framePeriod() {
    return 1000 * videoStream_->codec->time_base.num / videoStream_->codec->time_base.den;
}

bool VideoPlayer::timestamps() const {
    return timestamps_;
}

void VideoPlayer::timestamps(bool value) {
    timestamps_ = value;
}

void VideoPlayer::initFilters(const std::string &filterDescription) {
    avfilter_register_all();

    filterGraph_ = AVFilterGraphPtr(avfilter_graph_alloc(), [](AVFilterGraph * graph) {
        avfilter_graph_free(&graph);
    });

    char args[512];

    /* 
     * Buffer video source: the decoded frames from the decoder will be inserted here.
     */
    snprintf(args, sizeof (args), "%d:%d:%d:%d:%d:%d:%d",
            videoStream_->codec->width, videoStream_->codec->height, videoStream_->codec->pix_fmt,
            videoStream_->codec->time_base.num, videoStream_->codec->time_base.den,
            videoStream_->codec->sample_aspect_ratio.num, videoStream_->codec->sample_aspect_ratio.den);

    auto srcBufferPtr = bufferSrc_.get();
    if (avfilter_graph_create_filter(&srcBufferPtr, avfilter_get_by_name("buffer"), "src", args, NULL, filterGraph_.get()) < 0) {
        throw runtime_error("Cannot create buffer source.");
    }
    bufferSrc_ = AVFilterContextPtr(srcBufferPtr, [](AVFilterContext * filter) {
    }); // empty body = no deallocation

    /*
     * Buffer video sink: to terminate the filter chain. 
     */
    auto sinkBufferPtr = bufferSink_.get();
    if (avfilter_graph_create_filter(&sinkBufferPtr, avfilter_get_by_name("buffersink"), "out", NULL, NULL, filterGraph_.get()) < 0) {
        throw runtime_error("Cannot create buffer sink.");
    }
    bufferSink_ = AVFilterContextPtr(sinkBufferPtr, [](AVFilterContext * filter) {
    }); // empty body = no deallocation

    /*
     * Endpoints for the filter graph.
     */
    AVFilterInOutPtr outputs = AVFilterInOutPtr(avfilter_inout_alloc(), [](AVFilterInOut * inout) {
        avfilter_inout_free(&inout);
    });
    AVFilterInOutPtr inputs = AVFilterInOutPtr(avfilter_inout_alloc(), [](AVFilterInOut * inout) {
        avfilter_inout_free(&inout);
    });

    outputs->name = av_strdup("in");
    outputs->filter_ctx = bufferSrc_.get();
    outputs->pad_idx = 0;
    outputs->next = NULL;
    inputs->name = av_strdup("out");
    inputs->filter_ctx = bufferSink_.get();
    inputs->pad_idx = 0;
    inputs->next = NULL;

    //TODO don't know why .get() instead of .release() doesn't work... (segfaults at the end)
    if (avfilter_graph_parse(filterGraph_.get(), filterDescription.c_str(), inputs.release(), outputs.release(), NULL) < 0) {
        throw runtime_error("Cannot parse.");
    }

    if (avfilter_graph_config(filterGraph_.get(), NULL) < 0) {
        throw runtime_error("Cannot config.");
    }
}


