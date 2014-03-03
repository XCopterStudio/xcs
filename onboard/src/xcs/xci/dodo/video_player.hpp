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

#include <xcs/nodes/xobject/syntactic_types.hpp>


extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
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
        if (ctxt) reset(ctxt);
    }

    Packet(Packet&& other) : packet(std::move(other.packet)) {
        other.packet.data = nullptr;
    }

    ~Packet() {
        if (packet.data)
            av_free_packet(&packet);
    }

    void reset(AVFormatContext* ctxt) {
        if (packet.data)
            av_free_packet(&packet);
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
    void init(const std::string &filename);
    xcs::nodes::BitmapType getFrame();

private:
    AVStream* videoStream_;
    size_t videoStreamIndex_;
    std::shared_ptr<AVFormatContext> avFormat_;
    std::shared_ptr<AVCodecContext> avVideoCodec_;
    std::shared_ptr<AVFrame> avFrame_;
    size_t offsetInData_;
    AVPicture pic_;
    bool hasPic_;
    size_t frameCnt_;



};


}
}
}

#endif	/* VIDEOPLAYER_HPP */

