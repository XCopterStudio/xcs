set(PROJECT_NAME xobject)
project(${PROJECT_NAME})

file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.hpp)
file(GLOB SCRIPTS *.u) # TODO find better solution to include .u files in MSVC projects # TODO find better solution to include .u files in MSVC projects

x_add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

x_target_link_libraries(${PROJECT_NAME} xcs)

find_package(Xurbi)
use_urbi(${PROJECT_NAME})





