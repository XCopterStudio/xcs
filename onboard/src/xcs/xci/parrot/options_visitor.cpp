#include <iostream>

#include "options_visitor.hpp"
#include "navdata_options.hpp"

using namespace xcs::xci::parrot;

void OptionVisitor::visit(NavdataDemo* demo){
    printf("\r Navdata demo [%f,%f,%f]",demo->phi,demo->theta,demo->psi);
}

void OptionVisitor::visit(NavdataCks* cks){

}