set(PROJECT_NAME uxci)
project(${PROJECT_NAME})

file(GLOB SOURCES *.cpp)

add_library(${PROJECT_NAME} MODULE ${SOURCES})

x_target_link_libraries(${PROJECT_NAME} xcs)

find_package(Xurbi)
use_urbi(${PROJECT_NAME})





