#include "x_object.hpp"
#include "x.hpp"
#include <urbi/uobject.hh>
#include <boost/algorithm/string.hpp> //NOTE: must be under XVar.hpp and XcsObject.hpp

using namespace std;
using namespace urbi;
using namespace xcs::nodes;

XObject::XObject(const std::string& name) : UObject(name), xVarsType_(new map<const string, XType*>()) {
    XBindFunction(XObject, getType);
    XBindFunction(XObject, getSynType);
    XBindFunction(XObject, getSemType);
    XBindFunction(XObject, getXVars);
    XBindFunction(XObject, getXInputPorts);
}

XObject::~XObject(void) { 
    for (map<const string, XType*>::iterator it = xVarsType_->begin(); it != xVarsType_->end(); ++it) {
        delete it->second;
    }
    delete xVarsType_;
}

bool XObject::RegisterXVar(const string& xVarName, string synType, string semType) {
    return RegisterXChild(xVarName, synType, semType, XType::DATAFLOWTYPE_XVAR);
}

bool XObject::RegisterXInputPort(const string& xVarName, string synType, string semType) {
    return RegisterXChild(xVarName, synType, semType, XType::DATAFLOWTYPE_XINPUTPORT);
}

bool XObject::RegisterXChild(const string& xVarName, string synType, string semType, const XType::DataFlowType dataFlowType) {
    // erase whitespace in syntactic type
    //synType.erase(remove_if(synType.begin(), synType.end(), isspace), synType.end());

    // trim semantic type
    boost::algorithm::trim(semType);

    // reg new xvar
    if (xVarsType_->count(xVarName) == 0) {
        (*xVarsType_).emplace(xVarName, new XType(synType, semType, dataFlowType));
        return true;
    }

    return false;
}

const string XObject::getType(const string& xVarName) const {
    if(xVarsType_->count(xVarName) == 0) {
        return "";
    }

    return (*xVarsType_)[xVarName]->toString();
}

const string XObject::getSynType(const string& xVarName) const {
    if (xVarsType_->count(xVarName) == 0) {
        return "";
    }

    return (*xVarsType_)[xVarName]->synType;
}

const string XObject::getSemType(const string& xVarName) const {
    if (xVarsType_->count(xVarName) == 0) {
        return "";
    }

    return (*xVarsType_)[xVarName]->semType;
}

list<const string> XObject::getXVars() const {
    return getXChilds(XType::DATAFLOWTYPE_XVAR);
}

list<const string> XObject::getXInputPorts() const {
    return getXChilds(XType::DATAFLOWTYPE_XINPUTPORT);
}

list<const string> XObject::getXChilds(const XType::DataFlowType dataFlowType) const {
    list<const string> result;

    for (map<const string, XType*>::const_iterator it = xVarsType_->cbegin(); it != xVarsType_->cend(); ++it) {
        if (it->second->dataFlowType == dataFlowType) {
            result.push_back(it->first);
        }
    }

    return result;
}