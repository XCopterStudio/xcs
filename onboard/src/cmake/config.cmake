#
# Global configuration
#

# Paths
set(URBI_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../3rd-party/urbi)
set(URBI_D_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../3rd-party/urbi_d)
set(LIBAV_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../3rd-party/libav)
set(OPENCV_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../3rd-party/opencv)

set(BOOST_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../3rd-party/boost)
set(rapidjson_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../../3rd-party/rapidjson)

# Flags
set(Boost_USE_STATIC_LIBS false)
add_definitions(-DBOOST_ALL_NO_LIB) # disable autolinking (causing problems in MS VS)
