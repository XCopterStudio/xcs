#ifndef X_PTAM_H
#define X_PTAM_H

#include <xcs/nodes/xobject/x_object.hpp>


namespace xcs {
namespace nodes {

class XPtam : public XObject {
public:
    XPtam(const std::string &name);
    virtual ~XPtam();
};

}
}

#endif
