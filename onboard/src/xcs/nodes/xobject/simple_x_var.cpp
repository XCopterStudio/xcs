#include "simple_x_var.hpp"
#include <xcs/exception.hpp>

using namespace std;
using namespace urbi;
using namespace xcs::nodes;

SimpleXVar::SimpleXVar(const type_info& synT, const string& semT) :
    data_(NULL),
    xType_(synT, semT, XType::DATAFLOWTYPE_XVAR) {
}

SimpleXVar::~SimpleXVar() {
}

void SimpleXVar::Init(XObject& parent, const string& varname) {
    parent.RegisterXVar(varname, Type().synType, Type().semType);
    
    if(data_ != NULL) {
        delete data_;
    }
    data_ = new UVar(parent.__name, varname, parent.ctx_);
}

const XType& SimpleXVar::Type() const {
    return xType_;
}

urbi::UVar& SimpleXVar::Data() {
    if(data_ == NULL) {
        throw xcs::Exception("Null reference exception - call XBindVar(...) or Init(...) first");
    }
    return *data_;
}