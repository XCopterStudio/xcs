#include "x_type.hpp"

using namespace std;
using namespace xcs::nodes;

XType::XType(const string& synT, const string& semT, const DataFlowType dataFlowT) :
    synType(synT),
    semType(semT),
    dataFlowType(dataFlowT) {
}

XType::XType(const type_info& synT, const string& semT, const DataFlowType dataFlowT) :
    synType(synT.name()),
    semType(semT),
    dataFlowType(dataFlowT) {
}

bool XType::operator==(const XType& other) const {
    return semType ==  other.semType && synType == other.synType;
}

const string XType::toString() const {
    return synType + " - " + semType;
}