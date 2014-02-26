#ifndef XINPUTPORT_HPP
#define XINPUTPORT_HPP

#include <string>
#include "simple_xinput_port.hpp"
#include <xcs/nodes/xobject/xobject_export.h>

namespace xcs {
namespace nodes {

template<class synT>
class XOBJECT_EXPORT XInputPort : public SimpleXInputPort {
public:
    XInputPort(const std::string& semT);
    virtual ~XInputPort();
};

template<class synT>
XInputPort<synT>::XInputPort(const std::string& semT) :
    SimpleXInputPort(typeid(synT), semT) {
}

template<class synT>
XInputPort<synT>::~XInputPort() {
}

}
}

#endif