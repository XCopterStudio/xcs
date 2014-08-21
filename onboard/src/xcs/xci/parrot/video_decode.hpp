#ifndef VIDEO_DECODE_H
#define VIDEO_DECODE_H

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

//#pragma comment (lib, "avcodec.lib")
//#pragma comment (lib, "avformat.lib")

namespace xcs {
namespace xci {
namespace parrot {

/*! \brief Decoder for video frames from an AR.Drone 2.0.
            
    Can decode any video format which support libav and provide decoded frame as AVFrame with raw RGB image and same resolution as input frame.
    Class is not thread safe!
*/
class VideoDecoder {
    AVCodec* codec_;
    AVCodecContext * context_;
    AVFrame *frame_;
    AVFrame *frameOut_;
    SwsContext* swsContext_;
    uint8_t* bufferFrameOut;
public:
    /// Initialize data structures with null pointers.
    VideoDecoder() : codec_(nullptr), context_(nullptr), frame_(nullptr), frameOut_(nullptr), swsContext_(nullptr), bufferFrameOut(nullptr) {
    };
    /*! Create codec context which will be used for decoding input frames. 

        \param avCodec this codec will be used for decoding input frames. 
    */
    void init(const AVCodecID avCodec);
    /*! Decode input frame and store it in internal one frame buffer (rewrite last one in buffer).
        For acquiring image call decodedFrame().

        \param avpacket contains encoded video frame 
        \return true if decode process was successfully ended and in internal frame buffer is new frame
        \sa decodedFrame()
    */
    bool decodeVideo(AVPacket* avpacket);
    /*! Return last successfully decoded frame as RGB image with same resolution as last input encoded frame.
        For frame decoding call decodeVideo().
    
        /return pointer on internal one frame buffer with decoded frame
        /sa decodeVideo()
    */
    AVFrame* decodedFrame();

    //! Destroy allocated one frame buffers.
    ~VideoDecoder();
};

}
}
}
#endif
