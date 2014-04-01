#include "cartesian_vector.hpp"

using namespace xcs::nodes::xci;

CartesianVector::CartesianVector() :
    x(0),
    y(0),
    z(0){
}

CartesianVector::CartesianVector(double x, double y, double z) :
    x(x),
    y(y),
    z(z){
}

CartesianVectorChronologic::CartesianVectorChronologic() : 
    CartesianVector(),
    time(-1) {
}

CartesianVectorChronologic::CartesianVectorChronologic(double x, double y, double z, long int time) : 
    CartesianVector(x,y,z),
    time(-1) {

}

X_GENERATE_STRUCT(CartesianVectorChronologic, x, y, z, time);