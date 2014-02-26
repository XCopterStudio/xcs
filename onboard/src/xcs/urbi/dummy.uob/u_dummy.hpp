/* 
 * File:   UDummy.hpp
 * Author: michal
 *
 * Created on February 24, 2014, 20:22 PM
 */

#ifndef UDUMMY_HPP
#define	UDUMMY_HPP

#include <urbi/uobject.hh>


namespace xcs {
namespace urbi {

class UDummy : public ::urbi::UObject {
public:

    UDummy(const std::string &name);

    void init();

    virtual ~UDummy();
};

}
}

#endif	/* UDUMMY_HPP */

