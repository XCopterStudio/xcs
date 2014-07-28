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
    enum State {
        STATE_CREATED = 0, STATE_STARTED = 1, STATE_STOPPED = 2,
    };
    typedef std::list<std::string> StringList;
    XObject(const std::string& name);
    virtual ~XObject();
    void startXO();
    void stopXO();
    const std::string getSynType(const std::string& xVarName) const;
    const std::string getSemType(const std::string& xVarName) const;
    StringList getXVars() const;
    StringList getXInputPorts() const;
protected:
    bool registerXVar(const std::string& xVarName, const XType& type);
    bool registerXInputPort(const std::string& xVarName, const XType& type);
    // this method will never be called with STATE_CREATED parameter
    virtual void stateChanged(XObject::State state);
    const XObject::State getState() const;
private:
    typedef std::map<const std::string, XType> XTypesType;
    StringList getXChilds(const XType::DataFlowType dataFlowType) const;
    bool registerXChild(const std::string& xVarName, const XType& type, const XType::DataFlowType dataFlowType);
    void setState(XObject::State state);
    XTypesType xVarsType_;
    XObject::State state_;

    friend class SimpleXVar;
    friend class SimpleXInputPort;
};

}
}

#endif