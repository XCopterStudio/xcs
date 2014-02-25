#ifndef XCS_OBJECT_HPP
#define XCS_OBJECT_HPP

#include <string>
#include "XObject.hpp"

//TODO: prozatim se na nic nepouziva
class XcsObject : public XObject {
public:
    XcsObject(const std::string& name);
    virtual ~XcsObject();
private:
    static bool firstInstance_;
};

#endif