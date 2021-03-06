#include "video_receive.hpp"
#include <cassert>

#include <xcs/logging.hpp>

#include <boost/bind.hpp>
#include <iostream>

using namespace xcs::xci::parrot;
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace std;

const unsigned int VideoReceiver::TIMEOUT = 1000; // ms

const unsigned int VideoReceiver::BUFFER_COUNT = 30;
const unsigned int VideoReceiver::BUFFER_SIZE = 200000;

void VideoReceiver::handleConnectedVideo(const boost::system::error_code& ec){
    if (end_){
        return;
    }

    if (!socketVideo_.is_open()){
        XCS_LOG_WARN("Connect video socket timed out.");
        connect();
    } else if (ec){
        XCS_LOG_WARN("Receive video error: " + ec.message());
        socketVideo_.close();
        connect();
    } else {
        XCS_LOG_INFO("Video receive connected");
        receiveVideo();
    }
}

void VideoReceiver::receiveVideo(){
    if (end_){
        return;
    }

    
    deadlineVideo_.expires_from_now(boost::posix_time::millisec(TIMEOUT));

    if (!receivedHeader_){
        int index = sizeof(parrot_t)-receiveSize_;
        socketVideo_.async_receive(boost::asio::buffer(&((uint8_t *)&parrotPave_)[index], receiveSize_), boost::bind(&VideoReceiver::handleReceivedVideo, this, _1, _2));
    } else {
        int index = lastFrame_->payload_size - receiveSize_;
        socketVideo_.async_receive(boost::asio::buffer(&lastFrame_->data[index], receiveSize_), boost::bind(&VideoReceiver::handleReceivedVideo, this, _1, _2));
    }

}

void VideoReceiver::handleReceivedVideo(const boost::system::error_code& ec, std::size_t bytes_transferred){
    if (end_){
        return;
    }

    if (!socketVideo_.is_open()){
        XCS_LOG_WARN("Navdata video closed socket");
        connect();
    }else if (ec){
        XCS_LOG_WARN("Receive video data error: " + ec.message());
        socketVideo_.close();
        connect();
    } else {
        deadlineVideo_.expires_at(boost::posix_time::pos_infin);

        receiveSize_ -= bytes_transferred;
        if (receiveSize_ == 0){
            if (!receivedHeader_){
                receivedHeader_ = true;

                lastFrame_ = (VideoFrame*) &buffer_[index_*BUFFER_SIZE];
                // fill lastFrame data
                lastFrame_->width = parrotPave_.encoded_stream_width;
                lastFrame_->height = parrotPave_.encoded_stream_height;
                lastFrame_->video_codec = parrotPave_.video_codec;
                lastFrame_->payload_offset = parrotPave_.header_size - sizeof(parrot_t);
                lastFrame_->payload_size = parrotPave_.payload_size + lastFrame_->payload_offset;
                lastFrame_->frame_number = parrotPave_.frame_number;
                lastFrame_->frame_type = parrotPave_.frame_type;
                lastFrame_->timestamp = parrotPave_.timestamp / 1000.0;
                lastFrame_->data = (uint8_t*)&buffer_[index_*BUFFER_SIZE + sizeof(VideoFrame)];
                index_ = (index_ + 1) % BUFFER_COUNT;
                //cerr << "index " << index << endl;
                //lastFrame_->data = new uint8_t[lastFrame_->payload_size];

                receiveSize_ = lastFrame_->payload_size;
            } else { // received videoFrame
                receivedHeader_ = false;
                receiveSize_ = sizeof(parrot_t);

                if (checkPaveSignature(parrotPave_.signature)){
                    if ((++lastFrameNumber_ == parrotPave_.frame_number) || isIFrame(parrotPave_.frame_type)){
                        if (isIFrame(parrotPave_.frame_type) && !videoFrames_.empty()){
							XCS_LOG_WARN("Delete frames from video queue.");
                            XCS_LOG_WARN("End_ : " << end_);
                            std::vector<VideoFramePtr> frames = videoFrames_.popAll();
                            /*for (auto i : frames){
                                delete i;
                            }*/
                        }

                        lastFrameNumber_ = parrotPave_.frame_number;
                        //cerr << "Push video frame" << endl;
                        videoFrames_.push(lastFrame_);
                    }
                    else {
                        //delete lastFrame_;
                    }
                    lastFrame_ = nullptr;
                }
            }
        }
        
        receiveVideo();
    }
}

bool VideoReceiver::checkPaveSignature(uint8_t signature[4]) {
    return signature[0] == 'P' && signature[1] == 'a' && signature[2] == 'V' && signature[3] == 'E';
}

bool VideoReceiver::isIFrame(uint8_t frameType){
    return frameType == FRAME_TYPE_I_FRAME || frameType == FRAME_TYPE_IDR_FRAME;
}

void VideoReceiver::checkVideoDeadline(){
    if (end_){
        return;
    }

    // Check whether the deadline has passed. We compare the deadline against
    // the current time since a new asynchronous operation may have moved the
    // deadline before this actor had a chance to run.
    if (deadlineVideo_.expires_at() <= deadline_timer::traits_type::now())
    {
        // The deadline has passed. The socket is closed so that any outstanding
        // asynchronous operations are cancelled.
        socketVideo_.close();
        //connect();
        // There is no longer an active deadline. The expiry is set to positive
        // infinity so that the actor takes no action until a new deadline is set.
        deadlineVideo_.expires_at(boost::posix_time::pos_infin);
    }

	deadlineVideo_.async_wait(boost::bind(&VideoReceiver::checkVideoDeadline, this));
}


// ===================== public functions ========================================

VideoReceiver::VideoReceiver(boost::asio::io_service& io_service, std::string ipAddress, unsigned int port)
: deadlineVideo_(io_service),
socketVideo_(io_service),
parrotVideo(address::from_string(ipAddress), port){
    end_ = false;

    index_ = 0;
    buffer_ = new char[BUFFER_COUNT*BUFFER_SIZE];

    lastFrame_ = nullptr;
    lastFrameNumber_ = 0;
	deadlineVideo_.expires_at(boost::posix_time::pos_infin);
	deadlineVideo_.async_wait(boost::bind(&VideoReceiver::checkVideoDeadline, this));
}

VideoReceiver::~VideoReceiver(){
	end_ = true;
	socketVideo_.close();
    delete buffer_;
	/*vector<VideoFramePtr> frames = videoFrames_.popAll();
	for (auto frame : frames){
		delete frame;
	}*/
}

void VideoReceiver::connect(){
    if (end_){
        return;
    }

    if (lastFrame_ != nullptr){
        lastFrame_ = nullptr;
    }

    XCS_LOG_INFO("Try connect video receiver.");

    receivedHeader_ = false;
    receiveSize_ = sizeof(parrot_t);

    deadlineVideo_.expires_from_now(boost::posix_time::millisec(TIMEOUT));

	socketVideo_.open(tcp::v4());
    socketVideo_.async_connect(parrotVideo,
        boost::bind(&VideoReceiver::handleConnectedVideo, this, _1));
}

bool VideoReceiver::tryGetVideoFrame(VideoFramePtr& videoFrame){
    return videoFrames_.tryPop(videoFrame);
}