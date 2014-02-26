set(PROJECT_NAME xci_dodo)
project(${PROJECT_NAME})

file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.hpp)

add_library(${PROJECT_NAME} MODULE ${SOURCES} ${HEADERS})

find_package(UrbiAll)
use_urbi(${PROJECT_NAME})
