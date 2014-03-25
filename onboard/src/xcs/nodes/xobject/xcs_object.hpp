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
        std::string getStructs4Reg();
    private: /*static*/
        static std::map< std::string, std::vector<std::string>* >* structHolder_;
        static std::map< std::string, std::vector<std::string>* >* getStructHolder();
    public: /*static*/
        static int registerStructProperty(std::string structName, std::string structProperty);        
};

}
}

#endif