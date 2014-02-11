set(PROJECT_NAME xci-parrot)
project(${PROJECT_NAME})

file(GLOB SOURCES *.cpp)

include_directories(${Boost_INCLUDE_DIR})
add_library(${PROJECT_NAME} SHARED ${SOURCES})

