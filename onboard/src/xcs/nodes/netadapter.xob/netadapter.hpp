#ifndef NETADAPTER_HPP
#define NETADAPTER_HPP

#include <xcs/nodes/xobject/x_object.hpp>
#include <xcs/nodes/xobject/x_input_port.hpp>

namespace xcs {
namespace nodes {

class VideoWriter;

class NetAdapter : public XObject {
public:
    NetAdapter(const std::string &name);
    ~NetAdapter();
    void init(const std::string &location, const std::string &mimetype);

    XInputPort<::urbi::UImage> video;

private:
    void onChangeVideo(::urbi::UImage frame);
    VideoWriter* videoWriter_;
};

}
}

#endif // NETADAPTER_HPP
