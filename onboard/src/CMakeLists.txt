cmake_minimum_required(VERSION 2.8)
project(xcs)

#
# CMake modules
#
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(config)
include(platforms)
include(xlibrary)

#
# Include dirs
#
include_directories(${CMAKE_CURRENT_SOURCE_DIR})


#
# Subprojects
#
add_subdirectory(xcs)
add_subdirectory(xcs/xci/parrot)
add_subdirectory(xcs/xci/dodo)
#add_subdirectory(xcs/onboard)

# Subprojects (UObjects)
add_subdirectory(xcs/urbi/xci.uob)
add_subdirectory(xcs/urbi/json.uob)
add_subdirectory(xcs/urbi/dummy.uob)

