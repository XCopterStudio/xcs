#ifndef SIMPLE_XVAR_HPP
#define SIMPLE_XVAR_HPP

#include <string>
#include <typeinfo>
#include "x_object.hpp"
#include "x_type.hpp"
#include <xcs/nodes/xobject/xobject_export.h>
#include <xcs/exception.hpp>

namespace xcs {
namespace nodes {

class XOBJECT_EXPORT SimpleXVar {
public:
    SimpleXVar(const XType &type);
    virtual ~SimpleXVar();
    urbi::UVar& Data();
    const XType& Type() const;
    template<class T>
    SimpleXVar& operator=(const T&);
    void Init(xcs::nodes::XObject& parent, const std::string& varname);
private:
    urbi::UVar* data_;
    XType xType_;
};

template<class T>
SimpleXVar& SimpleXVar::operator=(const T& val) {
    Data() = val;
    return *this;
}

}
}

#endif