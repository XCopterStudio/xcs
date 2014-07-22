#ifndef XCS_X_H
#define XCS_X_H

#include <urbi/uvalue.hh>
#include <libport/containers.hh>

GD_CATEGORY(Test.All);

# define XBindVar(x) XBindVarRename(x, #x)

# define XBindPrivateVar(x) x.PrivateInit(*this, #x)

# define XBindVarRename(x, name) x.Init(*this, name)

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

#endif
