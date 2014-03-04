set(PROJECT_NAME xci_dodo)
project(${PROJECT_NAME})

file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.hpp)

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

find_package(UrbiAll)
use_urbi(${PROJECT_NAME})

find_package(LibAV)
include_directories(${LIBAV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${LIBAV_LIBRARIES})

