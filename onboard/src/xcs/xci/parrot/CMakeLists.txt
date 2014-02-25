set(PROJECT_NAME xci_parrot)
project(${PROJECT_NAME})


file(GLOB SOURCES *.cpp)

x_add_library(${PROJECT_NAME} ${SOURCES})

x_target_link_libraries(${PROJECT_NAME} xcs)

find_package(Xurbi)
use_urbi(${PROJECT_NAME})
