/* 
 * File:   Onboard.hpp
 * Author: michal
 *
 * Created on January 21, 2014, 8:49 PM
 */

#ifndef ONBOARD_HPP
#define	ONBOARD_HPP

#include <string>
#include "xcs/xci/XCI.hpp"

//#define Prop(T, X) \
//  private:  T _##X;  \
//  public:   T Get##X() { return _##X; } \
//  void Set##X(const T& x) { _##X = x; }

class Onboard {
public:

    enum State {
        UNDEFINED,
        INIT,
        SEROR,
    };

    Onboard();
    virtual ~Onboard();
    /*!
     * \param cmd  String name of the command (available command are listed in implementation).
     * \param ...  additional parameters depending on the command
     */
    bool DoCommand(const std::string& cmd, float roll = 0, float pitch = 0, float yaw = 0, float gaz = 0);
private:
    void init();
    xcs::xci::XCI* getXci();
    xcs::xci::XCI* xci;
    Onboard::State state;
};

#endif	/* ONBOARD_HPP */

