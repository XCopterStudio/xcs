/* 
 * File:   UXci.hpp
 * Author: michal
 *
 * Created on January 31, 2014, 11:07 PM
 */

#ifndef UXCI_HPP
#define	UXCI_HPP

#include <urbi/uobject.hh>
#include "xcs/xci/parrot/XCI_Parrot.hpp"

namespace xcs {
namespace urbi {

class UXci : public ::urbi::UObject {
public:
    UXci(const std::string &name);
    
    /*!
     * \param driver Name of the library with XCI implementation.
     */
    void init(const std::string &driver);
    
    void xciInit();
    
    void command(const std::string &command);
    
    void flyParam(double roll, double pitch, double yaw, double gaz);
    
    
    virtual ~UXci();
private:
    xcs::xci::XCI* xci_;
};

}
}

#endif	/* UXCI_HPP */

