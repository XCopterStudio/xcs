#include "x_object.hpp"
#include "x.h"
#include <urbi/uobject.hh>

using namespace std;
using namespace urbi;
using namespace xcs::nodes;

XObject::XObject(const std::string& name) : UObject(name) {
    XBindFunction(XObject, getType);
    XBindFunction(XObject, getSynType);
    XBindFunction(XObject, getSemType);
    XBindFunction(XObject, getXVars);
    XBindFunction(XObject, getXInputPorts);
    XBindFunction(XObject, startXO);
    XBindFunction(XObject, stopXO);
    XBindFunction(XObject, stateChanged);

    setState(XObject::STATE_CREATED);
}

XObject::~XObject() {
}

void XObject::startXO() {
    setState(XObject::STATE_STARTED);
}

void XObject::stopXO() {
    setState(XObject::STATE_STOPED);
}

bool XObject::RegisterXVar(const string& xVarName, const XType& type) {
    return RegisterXChild(xVarName, type, XType::DATAFLOWTYPE_XVAR);
}

bool XObject::RegisterXInputPort(const string& xVarName, const XType& type) {
    return RegisterXChild(xVarName, type, XType::DATAFLOWTYPE_XINPUTPORT);
}

bool XObject::RegisterXChild(const string& xVarName, const XType& type, const XType::DataFlowType dataFlowType) {
    // reg new xvar
    if (xVarsType_.count(xVarName) == 0) {
        xVarsType_.emplace(xVarName, type);
        return true;
    }

    return false;
}

const string XObject::getType(const string& xVarName) const {
    if(xVarsType_.count(xVarName) == 0) {
        return "";
    }

    return xVarsType_.at(xVarName).toString();
}

const string XObject::getSynType(const string& xVarName) const {
    if (xVarsType_.count(xVarName) == 0) {
        return "";
    }

    return xVarsType_.at(xVarName).synType;
}

const string XObject::getSemType(const string& xVarName) const {
    if (xVarsType_.count(xVarName) == 0) {
        return "";
    }

    return xVarsType_.at(xVarName).semType.name;
}

XObject::StringList XObject::getXVars() const {
    return getXChilds(XType::DATAFLOWTYPE_XVAR);
}

XObject::StringList XObject::getXInputPorts() const {
    return getXChilds(XType::DATAFLOWTYPE_XINPUTPORT);
}

XObject::StringList XObject::getXChilds(const XType::DataFlowType dataFlowType) const {
    XObject::StringList result;

    for (map<const string, XType>::const_iterator it = xVarsType_.cbegin(); it != xVarsType_.cend(); ++it) {
        if (it->second.dataFlowType == dataFlowType) {
            result.push_back(it->first);
        }
    }

    return result;
}

void XObject::setState(XObject::State state) {
    state_ = state;
    stateChanged(state_);
}

void XObject::stateChanged(int state) {
}

const XObject::State XObject::getState() const {
    return state_;
}