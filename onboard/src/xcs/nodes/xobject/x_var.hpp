#ifndef XVAR_HPP
#define XVAR_HPP

#include <string>
#include "simple_x_var.hpp"

namespace xcs {
namespace nodes {

template<class synT>
class XVar : public SimpleXVar {
public:
    XVar(const std::string& semT);
    XVar(const std::string& semT, const synT defVal);
    XVar& operator=(const synT&);
};

template<class synT>
XVar<synT>::XVar(const std::string& semT) :
    SimpleXVar(typeid(synT), semT) {
}

template<class synT>
XVar<synT>::XVar(const std::string& semT, const synT defVal) :
    SimpleXVar(typeid(synT), semT) {
    XVar::operator= (defVal);
}

template<class synT>
XVar<synT>& XVar<synT>::operator=(const synT& val) {
    Data() = val;
    return *this;
}

}
}

#endif
