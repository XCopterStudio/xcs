cmake_minimum_required(VERSION 2.8)
project(xcs)

#
# CMake modules
#
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(config)
include(common)
include(platforms)
include(xlibrary)

#
# Include dirs
#
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

#
# Setting binary variable
#
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#
# Subprojects
#
add_subdirectory(xcs)
add_subdirectory(xcs/xci/parrot)
add_subdirectory(xcs/xci/dodo)
add_subdirectory(xcs/xci/vrep)

# Subprojects (UObjects)
add_subdirectory(xcs/urbi/dummy.uob)
add_subdirectory(xcs/urbi/json.uob)
add_subdirectory(xcs/urbi/line_drawer.uob)
add_subdirectory(xcs/urbi/line_finder.uob)
add_subdirectory(xcs/urbi/line_keeper.uob)

# Subprojects (nodes)
add_subdirectory(xcs/nodes/ekf.xob)
add_subdirectory(xcs/nodes/control.xob)
add_subdirectory(xcs/nodes/datalogger.xob)
add_subdirectory(xcs/nodes/hermit_movement.xob)
add_subdirectory(xcs/nodes/onboard.xob)
add_subdirectory(xcs/nodes/xci.xob)
add_subdirectory(xcs/nodes/xobject)

# Subprojects (urbiscript)
#add_subdirectory(../urbiscript ./urbiscript)
