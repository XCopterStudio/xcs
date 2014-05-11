#ifndef XINPUTPORT_HPP
#define XINPUTPORT_HPP

#include <string>
#include "simple_x_input_port.hpp"

namespace xcs {
namespace nodes {

template<class synT>
class XInputPort : public SimpleXInputPort {
public:
    XInputPort(const SemanticTypeInfo& semT);
    virtual ~XInputPort();
};

template<class synT>
XInputPort<synT>::XInputPort(const SemanticTypeInfo& semT) :
    SimpleXInputPort(XType(typeid(synT), semT, XType::DATAFLOWTYPE_XINPUTPORT)) {
}

template<class synT>
XInputPort<synT>::~XInputPort() {
}

}
}

#endif