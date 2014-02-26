set(PROJECT_NAME xci_dodo)
project(${PROJECT_NAME})

file(GLOB SOURCES *.cpp)

add_library(${PROJECT_NAME} MODULE ${SOURCES})

find_package(Xurbi)
use_urbi(${PROJECT_NAME})
