set(PROJECT_NAME ekf)
project(${PROJECT_NAME})

file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.hpp)

add_library(${PROJECT_NAME} MODULE ${SOURCES} ${HEADERS})

x_target_link_libraries(${PROJECT_NAME} xobject)

find_package(UrbiAll)
use_urbi(${PROJECT_NAME})

find_package(Armadillo)
use_armadillo(${PROJECT_NAME})
