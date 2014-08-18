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

//! Information about video frame from parrot 2.0
struct VideoFrame{

    uint16_t width; //!< width of video frame
    uint16_t height; //!< height of video frame

    uint8_t video_codec; //!< video codec used for encoding of video frame
    uint32_t payload_size; //!< size of video data
    uint32_t frame_number; //!< number of frame in video stream
    uint8_t frame_type; //!< frame type (I-Frame or P-Frame)
    uint32_t payload_offset; //!< offset of video data
    double timestamp; //!< Timestamp of capturing frame according to the parrot 2.0 time 

    uint8_t *data; //!< video frame data

    VideoFrame() : data(nullptr) {};
};

typedef VideoFrame* VideoFramePtr;

class VideoReceiver{
    //! Define operations timeout for connection, received data etc.
    static const unsigned int TIMEOUT;

    //! Count of items in video buffer
    static const unsigned int BUFFER_COUNT;
    //! Size of one item in video buffer
    static const unsigned int BUFFER_SIZE;

    //! Pointer on first empty item in video buffer
    int index_;
    //! Store received video frames from parrot 2.0
    char* buffer_;

    //! Boost timer
    boost::asio::deadline_timer deadlineVideo_;
    //! Boost socket connection
    boost::asio::ip::tcp::socket socketVideo_;
    //! Boost ip endpoint
	boost::asio::ip::tcp::endpoint parrotVideo;

    //! Queue with pointers in to the buffer_ with received video frames
    Tsqueue<VideoFramePtr> videoFrames_;
  
    //! Variables for state machine which handle video frames receiving 
    bool receivedHeader_;
    bool differentHeaderSize_;

    //! Size of received part of video frame
    unsigned int receiveSize_;
    //! Parrot video header
    parrot_t parrotPave_;
    //! Keep data for actual receiving frame
    VideoFrame* lastFrame_;

    //! Number of last frame
    unsigned int lastFrameNumber_;

    //! Signalize end of video receiver
    volatile std::atomic<bool> end_;

    //! Handle connected video socket and call receive video
    void handleConnectedVideo(const boost::system::error_code& ec);
    //! Call boost receive for receiving video frame from parrot 2.0
    void receiveVideo();
    //! Process received video data from parrot 2.0
    void handleReceivedVideo(const boost::system::error_code& ec, std::size_t bytes_transferred);

    //! Test if input signature correspond pave signature
    /*!
        \param signature define type of video frame
        \return true if signature is equal 'P''A''V''E' otherwise false
    */
    bool checkPaveSignature(uint8_t signature[4]);
    //! Test type of frame
    /*!
        \param number corresponding to the frame type (see more in video_encapsulation.h)
        \return if frameType is equal to the IFrame or IDRFrame then return true otherwise false
    */
    bool isIFrame(uint8_t frameType);
    void checkVideoDeadline();
public:
    VideoReceiver(boost::asio::io_service& io_serviceVideo, std::string ipAdress = "192.168.1.1", unsigned int port = 5555);
	~VideoReceiver();
    void connect();
    bool tryGetVideoFrame(VideoFramePtr& videoFrame);
};

}}}
#endif