#ifndef X_HPP
#define X_HPP

#include <urbi/uobject.hh>
#include <libport/containers.hh>
#include "xcs_object.hpp"

# define XBindVar(x)							\
    this->RegisterXVar(#x, x.Type().synType, x.Type().semType);	\
    x.Init(*this, #x)

//TODO: prejmenovat na XBindVar (pretizit)
# define XBindVarF(x, f)	\
    XBindVar(x);			\
    UNotifyChange(x.Data(), f)

# define XBindFunction(xObj, f) UBindFunction(xObj, f)

//# define XNotifyChange(x, f) UNotifyChange(x.Data(), f)

# define XStart(type) UStart(type)

//TODO: prejmenovat na XStart (pretizit)
# define XStartRename(type, name) UStartRename(type, name)

# define X_REGISTER_STRUCT(cName, ...)          \
    URBI_REGISTER_STRUCT(cName, __VA_ARGS__)

#define X_GENERATE_STRUCT_PROPERTY_S(cName, cProperty) int _ ## cName ## _ ## cProperty ## _ = xcs::nodes::XcsObject::registerStructProperty(#cName, #cProperty)

#define X_GENERATE_STRUCT_PROPERTY(r, cName, cProperty) X_GENERATE_STRUCT_PROPERTY_S(cName, cProperty);

# define X_GENERATE_STRUCT(cName, ...)    \
    BOOST_PP_SEQ_FOR_EACH(X_GENERATE_STRUCT_PROPERTY, cName, LIBPORT_LIST(__VA_ARGS__, ))

#endif
