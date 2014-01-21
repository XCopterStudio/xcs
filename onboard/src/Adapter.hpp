/* 
 * File:   Adapter.hpp
 * Author: michal
 *
 * Created on January 21, 2014, 8:46 PM
 */

#ifndef ADAPTER_HPP
#define	ADAPTER_HPP

#include "Forest.hpp"

class Adapter {
public:
    Adapter(Forest & forest);
    virtual ~Adapter();
private:

};

#endif	/* ADAPTER_HPP */

