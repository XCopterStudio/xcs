#ifndef XVAR_HPP
#define XVAR_HPP

#include <string>
#include "simple_x_var.hpp"
#include <xcs/nodes/xobject/xobject_export.h>

namespace xcs {
namespace nodes {

template<class synT>
class XOBJECT_EXPORT XVar : public SimpleXVar {
public:
    XVar(const std::string& semT);
    XVar& operator=(const synT&);
};

template<class synT>
XVar<synT>::XVar(const std::string& semT) :
    SimpleXVar(typeid(synT), semT) {
}

template<class synT>
XVar<synT>& XVar<synT>::operator=(const synT& val) {
    Data() = val;
    return *this;
}

}
}

#endif