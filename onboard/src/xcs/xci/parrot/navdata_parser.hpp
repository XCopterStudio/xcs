#ifndef NAVDATA_PARSER_H
#define NAVDATA_PARSER_H

#include <vector>

#include "navdata_options.hpp"
#include "options_visitor.hpp"

namespace xcs{
namespace xci{
namespace parrot{

    class NavdataProcess{
    public:
        static uint32_t computeChecksum(Navdata* navdata ,const size_t size);
        static std::vector<OptionAcceptor*> parse(Navdata* navdata, uint32_t navdataCks, unsigned int lenght);
    };

}}}

#endif