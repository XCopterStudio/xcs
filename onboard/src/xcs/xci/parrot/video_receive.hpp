#ifndef VIDEO_RECEIVE_H
#define VIDEO_RECEIVE_H

#include <string>
#include <atomic>
#include <cstdint>
#include <memory>

#include "video_encapsulation.h"
#include "xcs/tsqueue.hpp"

#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>


namespace xcs{
namespace xci{
namespace parrot{

typedef parrot_video_encapsulation_t parrot_t;

struct VideoFrame{

    uint16_t width;
    uint16_t height;

    uint8_t video_codec;
    uint32_t payload_size;
    uint32_t frame_number;
    uint8_t frame_type;
    uint32_t payload_offset;

    uint8_t *data; 

    VideoFrame() : data(nullptr) {};
    ~VideoFrame() { if(data != nullptr) delete[] data; };
};

typedef VideoFrame* VideoFramePtr;

class VideoReceiver{
    static const unsigned int TIMEOUT;

    boost::asio::deadline_timer deadlineVideo_;
    boost::asio::ip::tcp::socket socketVideo_;
	boost::asio::ip::tcp::endpoint parrotVideo;

    Tsqueue<VideoFramePtr> videoFrames_;
  
    bool receivedHeader_;
    bool differentHeaderSize_;
    unsigned int receiveSize_;
    parrot_t parrotPave_;
    VideoFrame* lastFrame_;

    unsigned int lastFrameNumber_;

    volatile std::atomic<bool> end_;

    void handleConnectedVideo(const boost::system::error_code& ec);
    void receiveVideo();
    void handleReceivedVideo(const boost::system::error_code& ec, std::size_t bytes_transferred);

    bool checkPaveSignature(uint8_t signature[4]);
    bool isIFrame(uint8_t frameType);
    void checkVideoDeadline();
public:
    VideoReceiver(boost::asio::io_service& io_serviceVideo, std::string ipAdress, unsigned int port);
	~VideoReceiver();
    void connect();
    bool tryGetVideoFrame(VideoFramePtr& videoFrame);
};

}}}
#endif