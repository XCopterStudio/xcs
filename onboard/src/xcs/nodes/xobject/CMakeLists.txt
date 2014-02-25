set(PROJECT_NAME xobject)
project(${PROJECT_NAME})

file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.hpp)
file(GLOB SCRIPTS *.u)

x_add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${SCRIPTS})

x_target_link_libraries(${PROJECT_NAME} xcs)

find_package(Xurbi)
use_urbi(${PROJECT_NAME})





