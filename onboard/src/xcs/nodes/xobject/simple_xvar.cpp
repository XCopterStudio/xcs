#include "simple_xvar.hpp"
#include "xcs/Exception.hpp"

using namespace std;
using namespace urbi;
using namespace xcs::nodes;

SimpleXVar::SimpleXVar(const type_info& synT, const string& semT) :
    data_(NULL),
    xType_(synT, semT) {
}

SimpleXVar::~SimpleXVar() {
}

void SimpleXVar::Init(const UObject& parent, const string& varname) {
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