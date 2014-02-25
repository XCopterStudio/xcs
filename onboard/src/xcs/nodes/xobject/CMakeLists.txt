set(PROJECT_NAME xobject)
project(${PROJECT_NAME})

file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.hpp)

x_add_library(${PROJECT_NAME} MODULE ${SOURCES} ${HEADERS})

find_package(Xurbi)
use_urbi(${PROJECT_NAME})





