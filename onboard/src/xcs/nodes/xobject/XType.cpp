#include "XType.hpp"

using namespace std;

XType::XType(const string& synT, const string& semT) :
    synType(synT),
    semType(semT) {

}

XType::XType(const type_info& synT, const string& semT) :
    synType(synT.name()),
    semType(semT) {
}

bool XType::operator==(const XType& other) const {
    return semType ==  other.semType && synType == other.synType;
}

const string XType::toString() const {
    //TODO: upravit a zoptimalizovat vytvoreni nazvu
    return synType + " - " + semType;
}