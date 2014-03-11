#include "video_receive.hpp"
#include <cassert>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/bind.hpp>

using namespace xcs::xci::parrot;
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace std;

const unsigned int TIMEOUT = 1000; // ms

void VideoReceiver::receiveVideo(const boost::system::error_code& ec){
    cerr << "Video receive" << endl;

    if (end_){
        return;
    }

    if (ec){
    
    }else if (socketVideo_.is_open()){
        connected_ = true;

        if (!receivedHeader_){
            int index = sizeof(parrot_t)-receiveSize_;
            socketVideo_.async_receive(boost::asio::buffer(&((uint8_t *)&parrotPave_)[index], receiveSize_), boost::bind(&VideoReceiver::handleReceivedVideo, this, _1, _2));
        }
        else{
            int index = lastFrame_->payload_size - receiveSize_;
            socketVideo_.async_receive(boost::asio::buffer(&lastFrame_->data[index], receiveSize_), boost::bind(&VideoReceiver::handleReceivedVideo, this, _1, _2));
        }
    }
    else{
        // cannot open video port
    }
}

void VideoReceiver::handleReceivedVideo(const boost::system::error_code& ec, std::size_t bytes_transferred){
    cerr << "Video received" << endl;
    if (ec){
    
    }
    else{
        receiveSize_ -= bytes_transferred;
        if (receiveSize_ == 0){
            if (!receivedHeader_){
                receivedHeader_ = true;

                //assert(lastFrame_ != nullptr);
                lastFrame_ = new VideoFrame;
                // fill lastFrame data
                lastFrame_->width = parrotPave_.encoded_stream_width;
                lastFrame_->height = parrotPave_.encoded_stream_height;
                lastFrame_->video_codec = parrotPave_.video_codec;
                lastFrame_->payload_offset = parrotPave_.header_size - sizeof(parrot_t);
                lastFrame_->payload_size = parrotPave_.payload_size + lastFrame_->payload_offset;
                lastFrame_->frame_number = parrotPave_.frame_number;
                lastFrame_->frame_type = parrotPave_.frame_type;
                lastFrame_->data = new uint8_t[lastFrame_->payload_size];

                receiveSize_ = lastFrame_->payload_size;
            }
            else{ // received videoFrame
                receivedHeader_ = false;
                receiveSize_ = sizeof(parrot_t);

                if (checkPaveSignature(parrotPave_.signature)){
                    if ((++lastFrameNumber_ != parrotPave_.frame_number) || isIFrame(parrotPave_.frame_type)){
                        if (isIFrame(parrotPave_.frame_type) && videoFrames_.empty()){
                            std::vector<VideoFramePtr> frames = videoFrames_.popAll();
                            for (auto i : frames){
                                delete i;
                            }
                        }

                        lastFrameNumber_ = parrotPave_.frame_number;
                        videoFrames_.push(lastFrame_);
                    }
                    else{
                        delete lastFrame_;
                    }
                    lastFrame_ = nullptr;
                }
            }
        }
        
        receiveVideo(boost::system::error_code());
    }
}

bool VideoReceiver::checkPaveSignature(uint8_t signature[4]) {
    return signature[0] == 'P' && signature[1] == 'a' && signature[2] == 'V' && signature[3] == 'E';
}

bool VideoReceiver::isIFrame(uint8_t frameType){
    return frameType == FRAME_TYPE_I_FRAME || frameType == FRAME_TYPE_IDR_FRAME;
}

void VideoReceiver::checkVideoDeadline(){
    if (end_)
        return;

    // Check whether the deadline has passed. We compare the deadline against
    // the current time since a new asynchronous operation may have moved the
    // deadline before this actor had a chance to run.
    if (videoDeadline_.expires_at() <= deadline_timer::traits_type::now())
    {
        // The deadline has passed. The socket is closed so that any outstanding
        // asynchronous operations are cancelled.
        socketVideo_.close();

        // There is no longer an active deadline. The expiry is set to positive
        // infinity so that the actor takes no action until a new deadline is set.
        videoDeadline_.expires_at(boost::posix_time::pos_infin);
        connect(ipAdress_,port_);
    }

    // Put the actor back to sleep.
    videoDeadline_.async_wait(boost::bind(&VideoReceiver::checkVideoDeadline, this));
}


// ===================== public functions ========================================

VideoReceiver::VideoReceiver(boost::asio::io_service& io_serviceVideo) : videoDeadline_(io_serviceVideo), socketVideo_(io_serviceVideo){
    end_ = false;
    lastFrame_ = nullptr;
    connected_ = false;
    lastFrameNumber_ = 0;
}

void VideoReceiver::connect(std::string adress, int port){
    if (connected_){
        socketVideo_.close();
        connected_ = false;
    }

    if (lastFrame_ != nullptr){
        lastFrame_ = nullptr;
    }

    receivedHeader_ = false;
    receiveSize_ = sizeof(parrot_t);

    ipAdress_ = adress;
    port_ = port;

    tcp::endpoint parrotVideo(address::from_string(ipAdress_), port_);
    //videoDeadline_.expires_from_now(boost::posix_time::millisec(TIMEOUT));

    socketVideo_.async_connect(parrotVideo,
        boost::bind(&VideoReceiver::receiveVideo, this, _1));

    //videoDeadline_.async_wait(boost::bind(&VideoReceiver::checkVideoDeadline, this));
}

bool VideoReceiver::tryGetVideoFrame(VideoFramePtr videoFrame){
    return videoFrames_.tryPop(videoFrame);
}