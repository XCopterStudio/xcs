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

void SimpleXVar::initData(XObject& parent, const string& varname) {
    parent.registerXVar(varname, type());

    privateInitData(parent, varname);
}

void SimpleXVar::privateInitData(XObject& parent, const string& varname) {
    if (data_ != NULL) {
        delete data_;
    }
    data_ = new UVar(parent.__name, varname, parent.ctx_);
}

const XType& SimpleXVar::type() const {
    return xType_;
}

urbi::UVar& SimpleXVar::data() {
    if (data_ == NULL) {
        throw xcs::Exception("Null reference exception - call XBindVar(...) or initData(...) first");
    }
    return *data_;
}


