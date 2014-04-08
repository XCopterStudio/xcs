/* 
 * File:   VideoPlayer.hpp
 * Author: michal
 * \todo It's mostly copy+paste, refactor it to better cooperate with our video_decode in parrot.
 * 
 * Created on March 3, 2014, 11:47 AM
 */

#ifndef VIDEOPLAYER_HPP
#define	VIDEOPLAYER_HPP

#include <string>
#include <memory>
#include <functional>


#include <xcs/types/bitmap_type.hpp>


extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfiltergraph.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
}


namespace xcs {
namespace xci {
namespace dodo {

/*!
 * Auxiliar class for RIAA handling of AVPacket.
 */
struct Packet {

    explicit Packet(AVFormatContext* ctxt = nullptr) {
        av_init_packet(&packet);
        packet.data = nullptr;
        packet.size = 0;
        if (ctxt) {
            reset(ctxt);
        }
    }

    Packet(Packet&& other) : packet(std::move(other.packet)) {
        other.packet.data = nullptr;
    }

    ~Packet() {
        if (packet.data) {
            av_free_packet(&packet);
        }
    }

    void reset(AVFormatContext* ctxt) {
        if (packet.data) {
            av_free_packet(&packet);
        }
        if (av_read_frame(ctxt, &packet) < 0) {
            packet.data = nullptr;
            packet.size = 0;
        }
    }

    AVPacket packet;
};

class VideoPlayer {
public:
    VideoPlayer();
    void init(const std::string &filename, const std::string &fontFile);
    void rewind();
    xcs::BitmapType getFrame();
    size_t framePeriod();
    bool timestamps() const;
    void timestamps(bool value);

private:
    typedef std::unique_ptr<AVFormatContext, std::function<void (AVFormatContext *) >> AVFormatContextPtr;
    typedef std::unique_ptr<AVCodecContext, std::function<void (AVCodecContext *) >> AVCodecContextPtr;
    typedef std::unique_ptr<AVFrame, std::function<void (AVFrame *) >> AVFramePtr;

    typedef std::unique_ptr<AVFilterContext, std::function<void (AVFilterContext *) >> AVFilterContextPtr;
    typedef std::unique_ptr<AVFilterGraph, std::function<void (AVFilterGraph *) >> AVFilterGraphPtr;
    typedef std::unique_ptr<AVFilterInOut, std::function<void (AVFilterInOut *) >> AVFilterInOutPtr;

    /*
     * Stream
     */
    AVStream* videoStream_;
    size_t videoStreamIndex_;

    /*
     * Decoding
     */
    AVFormatContextPtr avFormat_;
    AVCodecContextPtr avVideoCodec_;
    AVFramePtr avFrame_;
    SwsContext *swsContext_;
    AVPicture pic_;
    bool hasPic_;
    bool timestamps_;

    /*
     * Filters
     */
    AVFilterContextPtr bufferSink_;
    AVFilterContextPtr bufferSrc_;
    AVFilterGraphPtr filterGraph_;

    void initFilters(const std::string &filterDescription);


};


}
}
}

#endif	/* VIDEOPLAYER_HPP */

