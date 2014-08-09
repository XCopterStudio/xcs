#ifndef NETADAPTER_HPP
#define NETADAPTER_HPP

#include <xcs/nodes/xobject/x_object.hpp>
#include <xcs/nodes/xobject/x_input_port.hpp>

// LibAV
extern "C" {
#include <libavformat/avformat.h>
}

namespace xcs {
namespace nodes {

//class WebrtcFacade;
class VideoWriter;

class NetAdapter : public XObject {
public:
    NetAdapter(const std::string &name);
    ~NetAdapter();
    void init(const std::string &location, const std::string &mimetype);

    XInputPort<::urbi::UImage> video;

private:
    void onChangeVideo(::urbi::UImage frame);

    AVFrame* avframe_;
    AVOutputFormat* outputFormat_;
    AVFormatContext* formatContext_;
    AVCodecContext* codecContext_;

    AVStream* stream_;
    AVPacket packet_;

    VideoWriter* videoWriter_;

    //WebrtcFacade* webrtcFacade_;
};

}
}

#endif // NETADAPTER_HPP
