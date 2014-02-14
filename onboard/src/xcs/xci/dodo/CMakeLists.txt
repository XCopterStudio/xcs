set(PROJECT_NAME xci-dodo)
project(${PROJECT_NAME})

include_directories(${Urbi_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIR})

file(GLOB SOURCES *.cpp)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

