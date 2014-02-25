#include "XObject.hpp"
#include <boost/algorithm/string.hpp> //NOTE: must be under XVar.hpp and XcsObject.hpp

using namespace std;
using namespace urbi;

XObject::XObject(const std::string& name) : UObject(name), xVarsType_(new map<const string, XType*>()) {
    XBindFunction(XObject, getType);
}

XObject::~XObject(void) { }

bool XObject::RegisterXVar(const string& xVarName, string synType, string semType) {
    //TODO: zjistit, jestli je rozdil v typeid().name na winech a linuxech a podle toho pripadne odkomentovat nasledujici
    // erase whitespace in syntactic type
    //synType.erase(remove_if(synType.begin(), synType.end(), isspace), synType.end());

    // trim semantic type
    boost::algorithm::trim(semType);	//TODO: zbavit se zavislosti na boostu

    //Todo: nahradit prirazeni za emplace (c++11)
    if(xVarsType_->count(xVarName) == 0) {
        (*xVarsType_)[xVarName] = new XType(synType, semType);
        return true;
    }

    //NOTE: takto se v pripade stejnych nazvu registruje pouze prvni
    //TODO: ZALOGOVAT/EXCEPTION?
    return false;
}

const string XObject::getType(const string& xVarName) const {
    if(xVarsType_->count(xVarName) == 0) {
        return "";
    }

    return (*xVarsType_)[xVarName]->toString();
}