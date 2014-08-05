#ifndef GENERALWRITER_HPP
#define	GENERALWRITER_HPP

#include <string>
#include <urbi/uobject.hh>

#include "abstract_writer.hpp"

namespace xcs {
namespace nodes {
namespace datalogger {

class ScalarWriter : public AbstractWriter {
public:
    ScalarWriter(const std::string &name);
    void init(const std::string &dataName, LoggerContext &context, ::urbi::UVar &uvar);

    void write(::urbi::UVar &uvar);

};


};
}
}


#endif	/* GENERALWRITER_HPP */

