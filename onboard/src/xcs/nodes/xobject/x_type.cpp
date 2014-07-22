#include "x_type.hpp"

using namespace std;
using namespace xcs::nodes;

XType::XType(const string& synT, const string& semT, const DataFlowType dataFlowT) :
    synType(synT),
    semType(semT), //NOTE: here's place for string to SemanticTypeInfo conversion
    dataFlowType(dataFlowT) {
}

XType::XType(const type_info& synT, const SemanticTypeInfo& semT, const DataFlowType dataFlowT) :
    synType(synT.name()),
    semType(semT),
    dataFlowType(dataFlowT) {
}

bool XType::operator==(const XType& other) const {
    return semType ==  other.semType && synType == other.synType;
}

bool XType::SemanticTypeInfo::operator ==(const SemanticTypeInfo& other) const {
    return name == other.name;
}