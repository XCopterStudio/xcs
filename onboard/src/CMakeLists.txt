cmake_minimum_required(VERSION 2.8)
project(xcs)

#
# Global configuration
#
set(Urbi_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../../3rd-party/urbi/include)
set(Urbi_LIBRARY_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../../3rd-party/urbi/lib)
set(BOOST_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../3rd-party/boost)
set(rapidjson_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../../3rd-party/rapidjson)

#
# Platform dispatch helpers
#
if(WIN32)
    set(X_WIN true)
    set(X_LIN false)
elseif(UNIX)
    set(X_WIN false)
    set(X_LIN true)
endif()

if(X_WIN)
  set (TPN_WIN32 "/EHsc")
  set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
  add_definitions(-D_WIN32_WINNT=0x0501)
elseif(X_LIN)
  set (CMAKE_CXX_FLAGS "-std=c++11")
endif()

#
# Linked libraries
#
# pthreads
find_package (Threads)

# Boost (TODO consider moving boost only to subprojects requiring them)
add_definitions(-DBOOST_ALL_NO_LIB) # disable autolinking (causing problems in MS VS)
set(Boost_USE_STATIC_LIBS false)
find_package(Boost 1.55 REQUIRED COMPONENTS system thread)
link_directories(${Urbi_LIBRARY_DIRS} ${BOOST_ROOT}/lib)

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
add_subdirectory(xcs/onboard)

# Subprojects (UObjects)
add_subdirectory(xcs/urbi/xci.uob)
add_subdirectory(xcs/urbi/json.uob)

