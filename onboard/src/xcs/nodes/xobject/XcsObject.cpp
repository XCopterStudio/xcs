#include <exception>
#include "XcsObject.hpp"
using namespace std;
using namespace urbi;

bool XcsObject::firstInstance_ = false;

XcsObject::XcsObject(const string& name) :
    XObject(name) {
    //TODO: vhodne zvolit chybovou hlasku
    // Singleton
    if(firstInstance_) {
        throw exception("Cannot create new XCS instance!!!");
    }
    firstInstance_ = true;
}

XcsObject::~XcsObject() {
}

UStartRename(XcsObject, XCS);