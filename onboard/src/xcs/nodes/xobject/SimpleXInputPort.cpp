#include "SimpleXInputPort.hpp"

using namespace urbi;
using namespace std;

SimpleXInputPort::SimpleXInputPort(const type_info& synT, const string& semT) :
    input_(NULL),
    xType_(synT, semT) {
}

SimpleXInputPort::~SimpleXInputPort() {
}

void SimpleXInputPort::Init(const UObject& parent, const string& varname) {
    // TODO: otestovat, co se stane, kdyz se smaze uz nabindovany uvar
    if(input_ != NULL) {
        delete input_;
    }
    input_ = new InputPort(parent.__name, varname, parent.ctx_);
}

const XType& SimpleXInputPort::Type() const {
    return xType_;
}

InputPort& SimpleXInputPort::Data() {
    // TODO: osetrit neinicializovany inputPort s daty
    if(input_ == NULL) {}
    return *input_;
}