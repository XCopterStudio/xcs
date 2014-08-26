#ifndef XCS_X_H
#define XCS_X_H

#include <urbi/uvalue.hh>
#include <libport/containers.hh>

GD_CATEGORY(Test.All);

# define XBindVar(x) XBindVarRename(x, #x)

# define XBindPrivateVar(x) x.privateInitData(*this, #x)

# define XBindVarRename(x, name) x.initData(*this, name)

# define XBindVarF(x, f)	\
    XBindVar(x);			\
    UNotifyChange(x.data(), f)

# define XBindFunction(xObj, f) UBindFunction(xObj, f)

# define XNotifyChange(x, f) UNotifyChange(x.data(), f)

# define XStart(type) UStart(type)

# define XStartRename(type, name) UStartRename(type, name)

# define X_REGISTER_STRUCT(cName, ...)          \
    URBI_REGISTER_STRUCT(cName, __VA_ARGS__)

#endif
