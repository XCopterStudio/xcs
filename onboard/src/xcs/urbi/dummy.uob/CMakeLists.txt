set(PROJECT_NAME udummy)
project(${PROJECT_NAME})


file(GLOB SOURCES *.cpp)

add_library(${PROJECT_NAME} MODULE ${SOURCES})

x_target_link_libraries(${PROJECT_NAME} xcs) # experiment for better linking on Windows, but don't know where the symbols could be referenced for

find_package(Xurbi)
use_urbi(${PROJECT_NAME})
