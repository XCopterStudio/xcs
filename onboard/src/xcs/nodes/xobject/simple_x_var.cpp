#include "simple_x_var.hpp"

using namespace std;
using namespace urbi;
using namespace xcs::nodes;

SimpleXVar::SimpleXVar(const XType &type) :
  data_(NULL),
  xType_(type) {
}

SimpleXVar::~SimpleXVar() {
}

void SimpleXVar::Init(XObject& parent, const string& varname) {
    parent.registerXVar(varname, Type());

    PrivateInit(parent, varname);
}

void SimpleXVar::PrivateInit(XObject& parent, const string& varname) {
    if (data_ != NULL) {
        delete data_;
    }
    data_ = new UVar(parent.__name, varname, parent.ctx_);
}

const XType& SimpleXVar::Type() const {
    return xType_;
}

urbi::UVar& SimpleXVar::Data() {
    if (data_ == NULL) {
        throw xcs::Exception("Null reference exception - call XBindVar(...) or Init(...) first");
    }
    return *data_;
}


