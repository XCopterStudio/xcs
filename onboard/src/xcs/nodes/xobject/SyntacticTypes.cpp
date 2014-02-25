#include "SyntacticTypes.hpp"
#include <urbi/uobject.hh>

namespace urbi {

void operator, (UValue& v, const xcs::nodes::BitmapType& t) {
    v.type = DATA_BINARY;
    // Here you must fill v with the serialized representation of t.
}

template<> struct uvalue_caster<xcs::nodes::BitmapType> {

    xcs::nodes::BitmapType operator()(UValue& v) {
        // Here you must return a MyType made with the information in v.
    }
};
}

