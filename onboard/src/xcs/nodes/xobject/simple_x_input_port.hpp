#ifndef SIMPLE_XINPUTPORT_HPP
#define SIMPLE_XINPUTPORT_HPP

#include <string>
#include <typeinfo>
#include "x_object.hpp"
#include "x_type.hpp"
#include <xcs/nodes/xobject/xobject_export.h>

namespace xcs {
namespace nodes {

class XOBJECT_EXPORT SimpleXInputPort {
public:
    SimpleXInputPort(const XType &type);
    virtual ~SimpleXInputPort();
    urbi::InputPort& data();
    const XType& type() const;
    void initData(xcs::nodes::XObject& parent, const std::string& varname);
    void privateInitData(xcs::nodes::XObject& parent, const std::string& varname);
private:
    urbi::InputPort* input_;
    XType xType_;
};

}
}

#endif