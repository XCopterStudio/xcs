#include "x_object.hpp"
#include "x.hpp"
#include <urbi/uobject.hh>

using namespace std;
using namespace urbi;
using namespace xcs::nodes;

XObject::XObject(const std::string& name) : UObject(name) {
    XBindFunction(XObject, getType);
}

XObject::~XObject(void) { 
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