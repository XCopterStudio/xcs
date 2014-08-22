#
# Global configuration
#

# Paths
if(USE_SYSTEM_LIBRARIES)
    set(PTAM_ROOT /opt/xcs-ptam)
else()
    set(BLAS_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../3rd-party/blas)
    set(LAPACK_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../3rd-party/lapack)
    set(LIBAV_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../3rd-party/libav)
    set(OPENCV_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../3rd-party/opencv)
    set(PTAM_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../3rd-party/ptam)
    set(URBI_D_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../3rd-party/urbi_d)
    set(URBI_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../3rd-party/urbi)
    set(BOOST_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../3rd-party/boost)
    set(Boost_NO_SYSTEM_PATHS 1)
endif()

# Nonstandard and header-only libraries that are used as git submodules.
set(rapidjson_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../../3rd-party/rapidjson)
set(armadillo_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../../3rd-party/armadillo/include)
set(VREP_REMOTE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../3rd-party/vrep-remote)


# Flags
set(Boost_USE_STATIC_LIBS false)
add_definitions(-DBOOST_ALL_NO_LIB) # disable autolinking (causing problems in MS VS)
