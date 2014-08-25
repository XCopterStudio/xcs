#ifndef X_RED_DOT_H
#define X_RED_DOT_H

#include <xcs/nodes/xobject/x_object.hpp>
#include <xcs/nodes/xobject/x_input_port.hpp>
#include <xcs/nodes/xobject/x_var.hpp>

#include "red_dot.hpp"

namespace xcs{
namespace nodes{
namespace reddot{

    class XRedDot : public XObject{
        RedDot redDot_;

        void onChangeVideo(::urbi::UImage image);
    public:
        XInputPort<::urbi::UImage> video;

        XVar<bool> found;
        XVar<int> errorX;
        XVar<int> errorY;

        XRedDot(const std::string &name);
    };

}}}

#endif 