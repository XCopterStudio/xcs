#ifndef GUIPROXY_HPP
#define GUIPROXY_HPP

#include <xcs/nodes/xobject/x_object.hpp>
#include <xcs/nodes/xobject/x_input_port.hpp>

namespace xcs {
namespace nodes {

class VideoWriter;

class GuiProxy : public XObject {
public:
    GuiProxy(const std::string &name);
    ~GuiProxy();
    void init(const std::string &location, const std::string &mimetype);

    XInputPort<::urbi::UImage> video;

private:
    void onChangeVideo(::urbi::UImage frame);
    VideoWriter* videoWriter_;
};

}
}

#endif // GUIPROXY_HPP
