#ifndef XCS_OBJECT_HPP
#define XCS_OBJECT_HPP

#include <string>
#include <map>
#include <vector>
#include "x_object.hpp"
#include "x_var.hpp"
#include <xcs/nodes/xobject/xobject_export.h>

namespace xcs {
namespace nodes {

class XOBJECT_EXPORT XcsObject : public XObject {
    public:
        XcsObject(const std::string& name);
        virtual ~XcsObject();
        int init();
        std::vector<std::string> getStructs4Reg();
        std::string getStructs4RegCode();
    private: /*static*/
        typedef std::map< std::string, std::vector<std::string> > HolderType;
        static HolderType structHolder_;
    public: /*static*/
        static int registerStructProperty(std::string structName, std::string structProperty);        
};

}
}

#endif