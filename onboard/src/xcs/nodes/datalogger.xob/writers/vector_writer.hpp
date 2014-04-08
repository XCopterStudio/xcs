#ifndef VectorWriter_HPP
#define	VectorWriter_HPP

#include <string>
#include <urbi/uobject.hh>

#include <xcs/nodes/xci.xob/structs/cartesian_vector.hpp>
#include <xcs/nodes/xci.xob/structs/eulerian_vector.hpp>
#include "abstract_writer.hpp"


namespace xcs {
namespace nodes {
namespace datalogger {

class VectorWriter : public AbstractWriter {
public:
    VectorWriter(const std::string &name);
    void init(const std::string &dataName, const TimePoint startTime, std::ofstream* file, std::mutex *lock, ::urbi::UVar &uvar);

    void write(xcs::nodes::xci::CartesianVectorChronologic value);
    
    //void write(::urbi::UDictionary value);
    
    //void write(xcs::nodes::xci::EulerianVectorChronologic value);
    //void write(std::string value);

};


};
}
}


#endif	/* VectorWriter_HPP */

