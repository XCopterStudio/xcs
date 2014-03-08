set(PROJECT_NAME xci_parrot)
project(${PROJECT_NAME})


file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.hpp)

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

x_target_link_libraries(${PROJECT_NAME} xcs)

find_package(UrbiAll)
use_urbi(${PROJECT_NAME})

find_package(LibAV)
message(STATUS "LIBAV: ${LIBAV_INCLUDE_DIRS}")
include_directories(${LIBAV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${LIBAV_LIBRARIES})

# TODO may be define global macro for logging
find_package(Boost 1.55 REQUIRED COMPONENTS log)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)

find_package (Threads)
target_link_libraries (${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
