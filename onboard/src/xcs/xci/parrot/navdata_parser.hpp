#ifndef NAVDATA_PARSER_H
#define NAVDATA_PARSER_H

#include <vector>
#include <cstddef>

#include "navdata_options.hpp"
#include "options_visitor.hpp"

namespace xcs{
namespace xci{
namespace parrot{

    //! Auxiliary class for navdata processing.
    class NavdataProcess{
    public:
        /*! Compute navdata packet checksum. 
            
            Checksum is computed as simple sum of packet represented as array of 32bit words.
            \param navdata pointer on navdata packet
            \param size is size of navdata packet on which pointer refer
        */
        static uint32_t computeChecksum(Navdata* navdata ,const size_t size);
        /*! Separate individual navdata option from navdata packet.
        
            \param navdata pointer on general navdata packet received from AR.Drone 2.0
            \param navdataCks is computed checksum of the navdata
            \param size is size of the navdata
            \return vector filled with navdata options from input navdata variable. 
            If navdataCks is not equal to the checksum in navdata packet return empty vector.
        */
        static std::vector<OptionAcceptor*> parse(Navdata* navdata, uint32_t navdataCks, unsigned int size);
    };

}}}

#endif