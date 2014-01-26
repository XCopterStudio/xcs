cmake_minimum_required(VERSION 2.8)
project(onboard)

# Configuration
set(libav_INSTALL ../3rd-party/libav/install)
set(Urbi_INCLUDE_DIRS ../3rd-party/urbi/include)

# Platform dependent stuff
if(WIN32)
  set(TPN_WIN32 "/EHsc")
endif(WIN32)

if(UNIX)
  set (CMAKE_CXX_FLAGS "-std=c++0x")
endif(UNIX)


# pthreads
find_package (Threads)

# Boost
add_definitions(-DBOOST_ALL_NO_LIB) # disable autolinking (causing problems in MS VS)
set(Boost_USE_STATIC_LIBS true)
find_package(Boost REQUIRED COMPONENTS system thread) # we don't specify version, asio seems to work for 1.46--1.54

# Include dirs
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${libav_INSTALL}/include)
include_directories(${Urbi_INCLUDE_DIRS})

# Library dirs
link_directories(${libav_INSTALL}/lib)

# Binaries
add_executable(main src/main.cpp src/Onboard.cpp src/XCILoader.cpp src/Adapter.cpp src/XCI_Parrot.cpp src/AT_Command.cpp src/video_decode.cpp)
add_library(xci SHARED src/XCI_Parrot.cpp)


# Linked libs
target_link_libraries(main ${Boost_LIBRARIES})
target_link_libraries (main ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(main avdevice avformat avfilter avcodec swscale avutil)
target_link_libraries(main dl m z)




