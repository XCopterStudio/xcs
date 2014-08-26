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
add_subdirectory(xcs/xci)
add_subdirectory(xcs/xci/parrot)
add_subdirectory(xcs/xci/dodo)
add_subdirectory(xcs/xci/vrep)

# Subprojects (UObjects)
# empty?

# Subprojects (nodes)
add_subdirectory(xcs/nodes/control.xob)
add_subdirectory(xcs/nodes/datalogger.xob)
add_subdirectory(xcs/nodes/dataplayer.xob)
add_subdirectory(xcs/nodes/flycontrol_aggregator.xob)
add_subdirectory(xcs/nodes/guiproxy.xob)
add_subdirectory(xcs/nodes/checkpoint_movement.xob)
add_subdirectory(xcs/nodes/line_drawer.xob)
add_subdirectory(xcs/nodes/line_finder.xob)
add_subdirectory(xcs/nodes/line_keeper.xob)
add_subdirectory(xcs/nodes/multiplexer.xob)
add_subdirectory(xcs/nodes/pid.xob)
add_subdirectory(xcs/nodes/localization.xob)
add_subdirectory(xcs/nodes/xci.xob)
add_subdirectory(xcs/nodes/xobject)
add_subdirectory(xcs/nodes/settings.xob)
add_subdirectory(xcs/nodes/red_dot.xob)
add_subdirectory(xcs/nodes/json.xob)


# Subprojects (urbiscript)
#add_subdirectory(../urbiscript ./urbiscript)
