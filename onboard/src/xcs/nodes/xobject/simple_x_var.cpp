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
    parent.RegisterXVar(varname, Type());
    
    if(data_ != NULL) {
        delete data_;
    }
    data_ = new UVar(parent.__name, varname, parent.ctx_);
}

const XType& SimpleXVar::Type() const {
    return xType_;
}

