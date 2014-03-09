cmake_minimum_required(VERSION 2.8) # minimum cmake version for opencv
set(PROJECT_NAME ulinefinder)
project(${PROJECT_NAME})

file(GLOB SOURCES *.cpp)

add_library(${PROJECT_NAME} MODULE ${SOURCES})

x_target_link_libraries(${PROJECT_NAME} xcs)

find_package(UrbiAll)
use_urbi(${PROJECT_NAME})

# opencv
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
target_link_libraries( ulinefinder ${OpenCV_LIBS} )

# TODO may be define global macro for logging
find_package(Boost 1.55 REQUIRED COMPONENTS log)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)

find_package (Threads)
target_link_libraries (${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
