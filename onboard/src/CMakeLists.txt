cmake_minimum_required(VERSION 2.8)
project(onboard)

# Configuration
set(ffmpeg_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../3rd-party/ffmpeg/build)
set(Urbi_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../../3rd-party/urbi/include)

set(main_SOURCES
    main.cpp
    Onboard.cpp XCILoader.cpp Adapter.cpp
    xcs/Exception.cpp xcs/ParseException.cpp
    xcs/LibraryLoader.cpp
)

set(xci_SOURCES
    xcs/xci/parrot/XCI_Parrot.cpp xcs/xci/parrot/AT_Command.cpp
)

# Platform dependent stuff
if(WIN32)
  set (TPN_WIN32 "/EHsc")
  set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
  add_definitions(-D_WIN32_WINNT=0x0501)
endif(WIN32)

if(UNIX)
  set (CMAKE_CXX_FLAGS "-std=c++11")
endif(UNIX)


# pthreads
find_package (Threads)

# Boost
add_definitions(-DBOOST_ALL_NO_LIB) # disable autolinking (causing problems in MS VS)
set(Boost_USE_STATIC_LIBS false)
find_package(Boost REQUIRED COMPONENTS system thread) # we don't specify version, asio seems to work for 1.46--1.54

# Include dirs
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${ffmpeg_ROOT}/include)
include_directories(${Urbi_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Library dirs
link_directories(${ffmpeg_ROOT}/lib)

# Binaries & their static libraries
add_executable(main ${main_SOURCES})
target_link_libraries(main ${Boost_LIBRARIES})
target_link_libraries(main ${CMAKE_THREAD_LIBS_INIT})
if(UNIX)
	target_link_libraries(main dl)
endif(UNIX)

add_library(xci SHARED ${xci_SOURCES})
target_link_libraries(xci ${Boost_LIBRARIES})
if(UNIX)
	target_link_libraries(xci m z)
endif(UNIX)

# Removed
# target_link_libraries(xci avdevice avformat avfilter avcodec swscale avutil)












