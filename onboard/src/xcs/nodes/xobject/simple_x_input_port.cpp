#include "simple_x_input_port.hpp"
#include <xcs/exception.hpp>

using namespace urbi;
using namespace std;
using namespace xcs::nodes;

SimpleXInputPort::SimpleXInputPort(const XType &type) :
    input_(NULL),
    xType_(type) {
}

SimpleXInputPort::~SimpleXInputPort() {
}

void SimpleXInputPort::initData(XObject& parent, const string& varname) {
    parent.registerXInputPort(varname, type());
    
    privateInitData(parent, varname);
}

void SimpleXInputPort::privateInitData(XObject& parent, const string& varname) {
    if (input_ != NULL) {
        delete input_;
    }
    input_ = new InputPort(parent.__name, varname, parent.ctx_);
}

const XType& SimpleXInputPort::type() const {
    return xType_;
}

InputPort& SimpleXInputPort::data() {
    if (input_ == NULL) {
        throw xcs::Exception("Null reference exception - call XBindVar(...) or initData(...) first");
    }
    return *input_;
}