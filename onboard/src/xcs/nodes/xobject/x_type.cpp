#include "x_type.hpp"

#include <cstdlib>
#include <memory>


using namespace std;
using namespace xcs::nodes;

XType::XType(const string& synT, const string& semT, const DataFlowType dataFlowT) :
    synType(synT),
    semType(semT), //NOTE: here's place for string to SemanticTypeInfo conversion
    dataFlowType(dataFlowT) {
}

XType::XType(const type_info& synT, const SemanticTypeInfo& semT, const DataFlowType dataFlowT) :
    synType(demangle(synT.name())),
    semType(semT),
    dataFlowType(dataFlowT) {
}

bool XType::operator==(const XType& other) const {
    return semType ==  other.semType && synType == other.synType;
}

bool XType::SemanticTypeInfo::operator ==(const SemanticTypeInfo& other) const {
    return name == other.name;
}

#ifdef __GNUG__
#include <cxxabi.h>

std::string XType::demangle(const char* name) {

    int status = -4; // some arbitrary value to eliminate the compiler warning

    // enable c++11 by passing the flag -std=c++11 to g++
    std::unique_ptr<char, void(*)(void*)> res {
        abi::__cxa_demangle(name, NULL, NULL, &status),
        std::free
    };

    return (status==0) ? res.get() : name ;
}

#else
// TODO implement for MSVC
// see: http://stackoverflow.com/questions/281818/unmangling-the-result-of-stdtype-infoname
// see: http://stackoverflow.com/questions/13777681/demangling-in-msvc

// does nothing if not g++
std::string demangle(const char* name) {
    return name;
}

#endif