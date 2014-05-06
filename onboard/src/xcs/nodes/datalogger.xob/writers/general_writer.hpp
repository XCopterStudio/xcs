#ifndef GENERALWRITER_HPP
#define	GENERALWRITER_HPP

#include <string>
#include <urbi/uobject.hh>

#include "abstract_writer.hpp"

namespace xcs {
namespace nodes {
namespace datalogger {

// TODO rename to ScalarWriter
class GeneralWriter : public AbstractWriter {
public:
    GeneralWriter(const std::string &name);
    void init(const std::string &dataName, LoggerContext &context, ::urbi::UVar &uvar);

    void write(::urbi::UVar &uvar);

};


};
}
}


#endif	/* GENERALWRITER_HPP */

