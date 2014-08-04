#include <sstream>
#include "x_object.hpp"
#include "x.h"
#include "xcs_object.hpp"

using namespace std;
using namespace urbi;
using namespace xcs::nodes;

map< string, vector<string>* > XcsObject::structHolder_;

XcsObject::XcsObject(const string& name) :
XObject(name) {
    // binding
    XBindFunction(XcsObject, init);
    XBindFunction(XcsObject, getStructs4Reg);
}

XcsObject::~XcsObject() {
}

int XcsObject::init() {
    send("echo(\"Cannot create new XCS instance!!!\");");
    return -1;
}

string XcsObject::getStructs4Reg() {
    stringstream structs;

    for (map<string, vector<string>*>::iterator it = structHolder_.begin(); it != structHolder_.end(); ++it) {
        // struct name
        structs << "class '" << it->first << "': UValueSerializable {" << endl;

        // struct properties
        for (string p : *(it->second)) {
            structs << "var " << p << ";" << endl;
        }

        // ctor
        structs << "function init(values = nil){" << endl;
        structs << "if(values){" << endl;
        for (string p : *(it->second)) {
            structs << "if(\"" << p << "\" in values) {" << endl;
            structs << "this." << p << " = values[\"" << p << "\"];};" << endl;
        }
        structs << "};"; 
        structs << "};"; // end of ctor


        // struct end
        structs << "}|;" << endl;
        structs << "var Serializables.'" << it->first << "' = '" << it->first << "'|;" << endl;

        delete it->second;
    }

    structHolder_.clear();
    return structs.str();
}

int XcsObject::registerStructProperty(string structName, string structProperty) {
    if (structHolder_.count(structName) == 0) {
        structHolder_.emplace(structName, new vector<string>);
    }

    structHolder_[structName]->push_back(structProperty);

    return 1;
}

XStartRename(xcs::nodes::XcsObject, XCS);