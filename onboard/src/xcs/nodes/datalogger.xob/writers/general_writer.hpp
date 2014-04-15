#ifndef GENERALWRITER_HPP
#define	GENERALWRITER_HPP

#include <string>
#include <urbi/uobject.hh>

#include "abstract_writer.hpp"

namespace xcs {
namespace nodes {
namespace datalogger {

class GeneralWriter : public AbstractWriter {
public:
    GeneralWriter(const std::string &name);
    virtual void start();
    void write(::urbi::UVar &uvar);

};


};
}
}


#endif	/* GENERALWRITER_HPP */

