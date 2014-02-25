set(PROJECT_NAME xcs)
project(${PROJECT_NAME})

file(GLOB SOURCES *.cpp)

x_add_library(${PROJECT_NAME} ${SOURCES})

if(X_LIN)
    target_link_libraries(${PROJECT_NAME} dl)
endif(X_LIN)

find_package(LibAV)
message(STATUS "LIBAV: ${LIBAV_INCLUDE_DIRS}")
message(STATUS "LIBAV libs: ${LIBAV_LIBRARIES}")
include_directories(${LIBAV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${LIBAV_LIBRARIES})
