#ifndef XTYPE_HPP
#define XTYPE_HPP

#include <string>
#include <typeinfo>
#include "xcs/nodes/xobject/xobject_export.h"

namespace xcs {
namespace nodes {

struct XOBJECT_EXPORT XType {
    const std::string synType;
    const std::string semType;
    XType(const std::string& synT, const std::string& semT);
    XType(const std::type_info& synType, const std::string& semT);
    bool operator==(const XType&) const;
    const std::string toString() const;
};

}
}

#endif