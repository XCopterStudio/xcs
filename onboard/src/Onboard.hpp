/* 
 * File:   Onboard.hpp
 * Author: michal
 *
 * Created on January 21, 2014, 8:49 PM
 */

#ifndef ONBOARD_HPP
#define	ONBOARD_HPP

#include <string>

//#define Prop(T, X) \
//  private:  T _##X;  \
//  public:   T Get##X() { return _##X; } \
//  void Set##X(const T& x) { _##X = x; }

class XCI;

class Onboard {
public:

    enum State {
        UNDEFINED,
        INIT,
        SEROR,
    };

    Onboard();
    virtual ~Onboard();
    bool DoCommand(const std::string& cmd);
private:
    void init();
    XCI* getXci();
    XCI* xci;
    Onboard::State state;
};

#endif	/* ONBOARD_HPP */

