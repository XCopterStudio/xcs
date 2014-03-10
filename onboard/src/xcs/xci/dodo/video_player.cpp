/* 
 * File:   VideoPlayer.cpp
 * Author: michal
 * \see http://blog.tomaka17.com/2012/03/libavcodeclibavformat-tutorial/
 * better \see http://www.sebastianroll.de/TableTopDoc/video_texture_8cpp-source.html#l00302
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

using namespace std;
using namespace xcs::nodes;
using namespace xcs::xci::dodo;

VideoPlayer::VideoPlayer() : hasPic_(false) {

}

void VideoPlayer::init(const string &filename) {
    // register all codecs
    av_register_all();

    // prepare context
    avFormat_ = shared_ptr<AVFormatContext>(avformat_alloc_context(), &avformat_free_context);

    // open file
    auto avFormatPtr = avFormat_.get();
    if (avformat_open_input(&avFormatPtr, filename.c_str(), nullptr, nullptr) != 0) {
        throw std::runtime_error("Error while calling avformat_open_input (probably invalid file format)");
    }

    if (avformat_find_stream_info(avFormat_.get(), nullptr) < 0) {
        throw std::runtime_error("Error while calling avformat_find_stream_info");
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
        throw std::runtime_error("Didn't find any video stream in the file (probably audio file)");
    }

    // find codec
    // getting the required codec structure
    const auto codec = avcodec_find_decoder(videoStream_->codec->codec_id);
    if (codec == nullptr)
        throw std::runtime_error("Codec required by video file not available");

    // allocating a structure
    avVideoCodec_ = shared_ptr<AVCodecContext> (avcodec_alloc_context3(codec), [](AVCodecContext * c) {
        avcodec_close(c); av_free(c); });

    // we need to make a copy of videoStream_->codec->extradata and give it to the context
    // make sure that this vector exists as long as the avVideoCodec exists
    vector<uint8_t> codecContextExtraData(videoStream_->codec->extradata, videoStream_->codec->extradata + videoStream_->codec->extradata_size);
    avVideoCodec_->extradata = codecContextExtraData.data();
    avVideoCodec_->extradata_size = codecContextExtraData.size();

    // initializing the structure by opening the codec
    if (avcodec_open2(avVideoCodec_.get(), codec, nullptr) < 0)
        throw std::runtime_error("Could not open codec");

    // prepare for decoding
    avFrame_ = shared_ptr<AVFrame>(avcodec_alloc_frame(), &av_free);
    cerr << "Inited avFrame to " << avFrame_.get() << endl;


    // data in the packet of data already processed
    offsetInData_ = 0;
    frameCnt_ = 0;



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

        // preparing the packet that we will send to libavcodec
        //        AVPacket packetToSend;
        //        packetToSend.data = packet_.packet.data + offsetInData_;
        //        packetToSend.size = packet_.packet.size - offsetInData_;
        //        cerr << "Sending " << (void*) packetToSend.data << ", size: " << packetToSend.size << endl;
        // sending data to libavcodec
        int isFrameAvailable = 0;
        const auto processedLength = avcodec_decode_video2(avVideoCodec_.get(), avFrame_.get(), &isFrameAvailable, &packet_.packet);
        if (processedLength < 0) {
            throw std::runtime_error("Error while processing the data");
        }
        offsetInData_ += processedLength;

        // processing the image if available
        if (isFrameAvailable) {
            // process image
            if (!hasPic_) {
                avpicture_alloc(&pic_, PIX_FMT_RGB24, avFrame_->width, avFrame_->height);
                hasPic_ = true;
            }
            auto ctxt = sws_getContext(avFrame_->width, avFrame_->height, static_cast<PixelFormat> (avFrame_->format), avFrame_->width, avFrame_->height, PIX_FMT_RGB24, SWS_BILINEAR, nullptr, nullptr, nullptr);
            if (ctxt == nullptr)
                throw std::runtime_error("Error while calling sws_getContext");
            sws_scale(ctxt, avFrame_->data, avFrame_->linesize, 0, avFrame_->height, pic_.data, pic_.linesize);

            BitmapType result;
            result.data = pic_.data[0];
            result.height = avFrame_->height;
            result.width = avFrame_->width;

            //cerr << "Dodo frame: " << frameCnt_++ << endl;
            //std::this_thread::sleep_for(std::chrono::milliseconds(200));
            return result;
            // TODO            avpicture_free(&pic);


            //            // sleeping until next frame
            //            const auto msToWait = avVideoCodec_->ticks_per_frame * 1000 * avVideoCodec_->time_base.num / avVideoCodec_->time_base.den;

        }
        failCounter++;

        //rewind video
        if(failCounter > 100) { // workaround for corrupted frames in stream
            av_seek_frame(avFormat_.get(), videoStreamIndex_, 0, 0);
            failCounter = 0;
        }
    } while (1);

    assert(false); // we should never get here
    BitmapType invalid;
    invalid.width = 0;
    invalid.height = 0;
    invalid.data = nullptr;
    cerr << "Did not get frame." << endl;

    return invalid;

}


