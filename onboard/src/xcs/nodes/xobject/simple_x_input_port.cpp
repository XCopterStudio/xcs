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

void SimpleXInputPort::Init(XObject& parent, const string& varname) {
    parent.registerXInputPort(varname, Type());
    
    PrivateInit(parent, varname);
}

void SimpleXInputPort::PrivateInit(XObject& parent, const string& varname) {
    if (input_ != NULL) {
        delete input_;
    }
    input_ = new InputPort(parent.__name, varname, parent.ctx_);
}

const XType& SimpleXInputPort::Type() const {
    return xType_;
}

InputPort& SimpleXInputPort::Data() {
    if (input_ == NULL) {
        throw xcs::Exception("Null reference exception - call XBindVar(...) or Init(...) first");
    }
    return *input_;
}