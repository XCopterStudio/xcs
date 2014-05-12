set(PROJECT_NAME xci_parrot)
project(${PROJECT_NAME})

file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.hpp)

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

x_target_link_libraries(${PROJECT_NAME} xcs)
x_target_link_libraries(${PROJECT_NAME} xobject)

find_package(UrbiAll)
use_urbi(${PROJECT_NAME})

find_package(LibAV)
message(STATUS "LIBAV: ${LIBAV_INCLUDE_DIRS}")
include_directories(${LIBAV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${LIBAV_LIBRARIES})

use_logging(${PROJECT_NAME})
