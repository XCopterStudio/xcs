#ifndef XOBJECT_HPP
#define XOBJECT_HPP

#include <urbi/uobject.hh>
#include <string>
#include <map>
#include "x_type.hpp"
#include <xcs/nodes/xobject/xobject_export.h>

namespace xcs {
namespace nodes {

class XOBJECT_EXPORT XObject : public urbi::UObject {
public:
    XObject(const std::string& name);
    virtual ~XObject();
    const std::string getType(const std::string& xVarName) const;
protected:
    bool RegisterXVar(const std::string& xVarName, const XType& type);
    bool RegisterXInputPort(const std::string& xVarName, const XType& type);
private:
    typedef std::map<const std::string, XType> XTypesType;
    
    bool RegisterXChild(const std::string& xVarName, const XType& type, const XType::DataFlowType dataFlowType);
    XTypesType xVarsType_;

    friend class SimpleXVar;
    friend class SimpleXInputPort;
};

}
}

#endif