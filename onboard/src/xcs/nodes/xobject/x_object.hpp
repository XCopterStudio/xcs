#ifndef XOBJECT_HPP
#define XOBJECT_HPP

#include <urbi/uobject.hh>
#include <string>
#include <map>
#include <list>
#include "x_type.hpp"
#include <xcs/nodes/xobject/xobject_export.h>

namespace xcs {
namespace nodes {

class XOBJECT_EXPORT XObject : public urbi::UObject {
public:
    typedef std::list<std::string> StringList;
    XObject(const std::string& name);
    virtual ~XObject();
    const std::string getType(const std::string& xVarName) const;
    const std::string getSynType(const std::string& xVarName) const;
    const std::string getSemType(const std::string& xVarName) const;
    StringList getXVars() const;
    StringList getXInputPorts() const;
protected:
    bool RegisterXVar(const std::string& xVarName, const XType& type);
    bool RegisterXInputPort(const std::string& xVarName, const XType& type);
private:
    typedef std::map<const std::string, XType> XTypesType;
    StringList getXChilds(const XType::DataFlowType dataFlowType) const;
    bool RegisterXChild(const std::string& xVarName, const XType& type, const XType::DataFlowType dataFlowType);
    XTypesType xVarsType_;

    friend class SimpleXVar;
    friend class SimpleXInputPort;
};

}
}

#endif