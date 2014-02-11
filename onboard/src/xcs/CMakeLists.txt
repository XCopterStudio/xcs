set(PROJECT_NAME xcs)
project(${PROJECT_NAME})

file(GLOB SOURCES *.cpp)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

