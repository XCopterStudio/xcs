#ifndef SIMPLE_XVAR_HPP
#define SIMPLE_XVAR_HPP

#include <string>
#include <typeinfo>
#include <urbi/uobject.hh>
#include "XType.hpp"

class SimpleXVar {
public:
    SimpleXVar(const std::type_info& synT, const std::string& semT);
    virtual ~SimpleXVar();
    urbi::UVar& Data();
    const XType& Type() const;
    template<class T>
    SimpleXVar& operator=(const T&);
    void Init(const urbi::UObject& parent, const std::string& varname);
protected:	// TODO: melo by stacit private
    urbi::UVar* data_;
    XType xType_;
};

template<class T>
SimpleXVar& SimpleXVar::operator=(const T& val) {
    Data() = val;
    return *this;
}

#endif