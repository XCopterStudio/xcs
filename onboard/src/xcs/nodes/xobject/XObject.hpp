#ifndef XOBJECT_HPP
#define XOBJECT_HPP

#include <urbi/uobject.hh>
#include <string>
#include <map>
#include "xtype.hpp"
#include "xcs/nodes/xobject/xobject_export.h"

namespace xcs {
namespace nodes {

class XOBJECT_EXPORT XObject : public urbi::UObject {
public:
    XObject(const std::string& name);
    virtual ~XObject();
    const std::string getType(const std::string& xVarName) const;
protected:
    bool RegisterXVar(const std::string& xVarName, std::string synType, std::string semType);
private:
    std::map<const std::string, XType*>* xVarsType_;
};

}
}

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

#endif