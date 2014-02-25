#include "SimpleXVar.hpp"
#include "XcsObject.hpp"

using namespace std;
using namespace urbi;

SimpleXVar::SimpleXVar(const type_info& synT, const string& semT) :
    data_(NULL),
    xType_(synT, semT) {
}

SimpleXVar::~SimpleXVar() {
}

void SimpleXVar::Init(const UObject& parent, const string& varname) {
    // TODO: otestovat, co se stane, kdyz se smaze uz nabindovany uvar
    if(data_ != NULL) {
        delete data_;
    }
    data_ = new UVar(parent.__name, varname, parent.ctx_);
}

const XType& SimpleXVar::Type() const {
    return xType_;
}

urbi::UVar& SimpleXVar::Data() {
    // TODO: osetrit neinicializovany uvar s daty
    if(data_ == NULL) {}
    return *data_;
}