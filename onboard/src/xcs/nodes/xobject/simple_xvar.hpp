#ifndef SIMPLE_XVAR_HPP
#define SIMPLE_XVAR_HPP

#include <string>
#include <typeinfo>
#include <urbi/uobject.hh>
#include "xtype.hpp"
#include "xcs/nodes/xobject/xobject_export.h"

namespace xcs {
namespace nodes {

class XOBJECT_EXPORT SimpleXVar {
public:
    SimpleXVar(const std::type_info& synT, const std::string& semT);
    virtual ~SimpleXVar();
    urbi::UVar& Data();
    const XType& Type() const;
    template<class T>
    SimpleXVar& operator=(const T&);
    void Init(const urbi::UObject& parent, const std::string& varname);
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