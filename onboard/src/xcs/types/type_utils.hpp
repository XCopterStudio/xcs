#ifndef CATEGORY_UTILS_HPP
#define	CATEGORY_UTILS_HPP

#include <libport/preproc.hh>
#include <map>
#include <string>

#include <xcs/types/bitmap_type.hpp>
#include <xcs/types/cartesian_vector.hpp>
#include <xcs/types/eulerian_vector.hpp>
#include <xcs/types/fly_control.hpp>
#include <xcs/types/speed_control.hpp>
#include <xcs/types/checkpoint.hpp>

#define XCS_SCALAR_TYPES LIBPORT_LIST( \
        double, \
        int, \
        std::string, )

#define XCS_VECTOR_TYPES LIBPORT_LIST( \
        xcs::EulerianVector, \
        xcs::EulerianVectorChronologic, \
        xcs::CartesianVector, \
        xcs::CartesianVectorChronologic, \
        xcs::FlyControl,\
        xcs::SpeedControl,\
        xcs::Checkpoint, )

#define XCS_VIDEO_TYPES LIBPORT_LIST( \
        xcs::BitmapType, )

namespace xcs {

enum SyntacticCategoryType {
    CATEGORY_SCALAR,
    CATEGORY_VECTOR,
    CATEGORY_VIDEO
};

typedef std::map<std::string, SyntacticCategoryType> SyntacticCategoryMap;

inline void fillTypeCategories(SyntacticCategoryMap &categoryMap) {
    if (!categoryMap.empty()) {
        return;
    }

#define FILTER(Type) categoryMap.emplace(#Type, CATEGORY_SCALAR);
    LIBPORT_LIST_APPLY(FILTER, XCS_SCALAR_TYPES);
#undef FILTER

#define FILTER(Type) categoryMap.emplace(#Type, CATEGORY_VECTOR);
    LIBPORT_LIST_APPLY(FILTER, XCS_VECTOR_TYPES);
#undef FILTER

#define FILTER(Type) categoryMap.emplace(#Type, CATEGORY_VIDEO);
    LIBPORT_LIST_APPLY(FILTER, XCS_VIDEO_TYPES);
#undef FILTER
}


}
#endif	/* CATEGORY_UTILS_HPP */

