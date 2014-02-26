#include "u_dummy.hpp"


using namespace xcs::urbi;

UDummy::UDummy(const std::string& name) : ::urbi::UObject(name) {
    UBindFunction(UDummy, init);
}

UDummy::~UDummy() {

}

void UDummy::init() {
    std::cerr << "Dummy uobject init'd." << std::endl;
}


UStart(UDummy);