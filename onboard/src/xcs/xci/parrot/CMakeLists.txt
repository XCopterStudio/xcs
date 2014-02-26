set(PROJECT_NAME xci_parrot)
project(${PROJECT_NAME})


file(GLOB SOURCES *.cpp)

add_library(${PROJECT_NAME} MODULE ${SOURCES})

x_target_link_libraries(${PROJECT_NAME} xcs)

find_package(Xurbi)
use_urbi(${PROJECT_NAME})

find_package(LibAV)
message(STATUS "LIBAV: ${LIBAV_INCLUDE_DIRS}")
include_directories(${LIBAV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${LIBAV_LIBRARIES})
