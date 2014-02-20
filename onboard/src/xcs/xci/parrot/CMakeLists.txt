set(PROJECT_NAME xci_parrot)
project(${PROJECT_NAME})


file(GLOB SOURCES *.cpp)

x_add_library(${PROJECT_NAME} ${SOURCES})

find_package(Xurbi)
use_urbi(${PROJECT_NAME})
