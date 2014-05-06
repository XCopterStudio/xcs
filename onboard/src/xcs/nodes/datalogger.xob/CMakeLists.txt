set(PROJECT_NAME xdatalogger)
project(${PROJECT_NAME})

file(GLOB SOURCES *.cpp writers/*.cpp)
file(GLOB HEADERS *.hpp writers/*.hpp)

add_library(${PROJECT_NAME} MODULE ${SOURCES} ${HEADERS})

x_target_link_libraries(${PROJECT_NAME} xobject)

find_package(UrbiAll)
use_urbi(${PROJECT_NAME})

find_package(LibAV)
include_directories(${LIBAV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${LIBAV_LIBRARIES})

use_logging(${PROJECT_NAME})