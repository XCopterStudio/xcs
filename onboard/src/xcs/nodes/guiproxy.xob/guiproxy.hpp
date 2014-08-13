#ifndef GUIPROXY_HPP
#define GUIPROXY_HPP

#include <xcs/nodes/xobject/x_object.hpp>
#include <xcs/nodes/xobject/x_input_port.hpp>

class AVFrame;

namespace xcs {
namespace nodes {

class VideoFileWriter;

class GuiProxy : public XObject {
public:
    GuiProxy(const std::string &name);
    ~GuiProxy();
    void init(const std::string &location, const std::string &mimetype);
    void initVideo();
    void deinitVideo();

    XInputPort<::urbi::UImage> video;

private:
    void onChangeVideo(::urbi::UImage frame);

    AVFrame* avframe_;
    VideoFileWriter* videoFileWriter_;
};

}
}

#endif // GUIPROXY_HPP
