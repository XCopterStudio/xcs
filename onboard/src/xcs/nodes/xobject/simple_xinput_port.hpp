#ifndef SIMPLE_XINPUTPORT_HPP
#define SIMPLE_XINPUTPORT_HPP

#include <string>
#include <typeinfo>
#include <urbi/uobject.hh>
#include "xtype.hpp"
#include "xcs/nodes/xobject/xobject_export.h"

namespace xcs {
namespace nodes {

class XOBJECT_EXPORT SimpleXInputPort {
public:
    SimpleXInputPort(const std::type_info& synT, const std::string& semT);
    virtual ~SimpleXInputPort();
    urbi::InputPort& Data();
    const XType& Type() const;
    void Init(const urbi::UObject& parent, const std::string& varname);
private:
    urbi::InputPort* input_;
    XType xType_;
};

}
}

#endif