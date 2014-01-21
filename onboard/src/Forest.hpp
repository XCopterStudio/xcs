/* 
 * File:   Forest.hpp
 * Author: michal
 *
 * Created on January 21, 2014, 8:49 PM
 */

#ifndef FOREST_HPP
#define	FOREST_HPP

#include <string>

class Forest {
public:
    Forest();
    Forest(const Forest& orig);
    
    void doCommand(std::string command);
    virtual ~Forest();
private:

};

#endif	/* FOREST_HPP */

