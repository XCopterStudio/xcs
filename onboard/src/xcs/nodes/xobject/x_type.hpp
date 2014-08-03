#ifndef XTYPE_HPP
#define XTYPE_HPP

#include <string>
#include <typeinfo>
#include <xcs/nodes/xobject/xobject_export.h>

namespace xcs {
namespace nodes {

class XOBJECT_EXPORT XType {
public:

    struct SemanticTypeInfo {
        const std::string name;

        SemanticTypeInfo(const std::string &name) : name(name) {
        }

        SemanticTypeInfo(const char *name) : name(name) {
        }

        bool operator==(const SemanticTypeInfo& other) const;

    };

    enum DataFlowType {
        DATAFLOWTYPE_XVAR, DATAFLOWTYPE_XINPUTPORT
    };

    const std::string synType;
    const SemanticTypeInfo semType;
    const DataFlowType dataFlowType;

    XType(const std::string& synT, const std::string& semT, const DataFlowType dataFlowT);
    XType(const std::type_info& synType, const SemanticTypeInfo& semT, const DataFlowType dataFlowT);
    bool operator==(const XType&) const;
private:
    std::string demangle(const char* name);
};

typedef XType::SemanticTypeInfo SemanticTypeInfo;

}
}

#endif