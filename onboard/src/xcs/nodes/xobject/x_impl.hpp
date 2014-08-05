#ifndef X_IMPL_HPP
#define X_IMPL_HPP

#include "x.h"
#include "xcs_object.hpp"


#define X_GENERATE_STRUCT_PROPERTY_S(cName, cProperty) int _ ## cName ## _ ## cProperty ## _ = xcs::nodes::XcsObject::registerStructProperty(X_STRUCT_NAMESPACE #cName, #cProperty)

#define X_GENERATE_STRUCT_PROPERTY(r, cName, cProperty) X_GENERATE_STRUCT_PROPERTY_S(cName, cProperty);

#define X_STRUCT_NAMESPACE ""
/**
 * \param cName Name must be valid as C++ variable name.
 */
# define X_GENERATE_STRUCT(cName, ...)    \
    BOOST_PP_SEQ_FOR_EACH(X_GENERATE_STRUCT_PROPERTY, cName, LIBPORT_LIST(__VA_ARGS__, ))

#endif
