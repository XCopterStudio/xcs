#ifndef XVAR_HPP
#define XVAR_HPP

#include <string>
#include "simple_x_var.hpp"

namespace xcs {
namespace nodes {

template<class synT>
class XVar : public SimpleXVar {
public:
    XVar(const SemanticTypeInfo& semT);
    XVar(const SemanticTypeInfo& semT, const synT defVal);
    XVar& operator=(const synT&);
};

template<class synT>
XVar<synT>::XVar(const SemanticTypeInfo& semT) :
    SimpleXVar(XType(typeid(synT), semT, XType::DATAFLOWTYPE_XVAR)) {
}

template<class synT>
XVar<synT>::XVar(const SemanticTypeInfo& semT, const synT defVal) :
    SimpleXVar(XType(typeid(synT), semT, XType::DATAFLOWTYPE_XVAR)) {
    XVar::operator= (defVal);
}

template<class synT>
XVar<synT>& XVar<synT>::operator=(const synT& val) {
    data() = val;
    return *this;
}

}
}

#endif
