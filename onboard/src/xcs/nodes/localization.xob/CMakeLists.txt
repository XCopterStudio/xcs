set(PROJECT_NAME xlocalization)
project(${PROJECT_NAME})

file(GLOB SOURCES *.cpp tum/*.cpp)
file(GLOB HEADERS *.hpp tum/*.h tum/*.hpp)

include_directories(${PTAM_ROOT}/include)

add_library(${PROJECT_NAME} MODULE ${SOURCES} ${HEADERS})

x_target_link_libraries(${PROJECT_NAME} xobject)

find_package(UrbiAll)
use_urbi(${PROJECT_NAME})

find_package(Armadillo)
use_armadillo(${PROJECT_NAME})

find_library(PTAM_LIBRARIES ptam PATHS ${PTAM_ROOT}/lib)
target_link_libraries(${PROJECT_NAME} ${PTAM_LIBRARIES})

use_logging(${PROJECT_NAME})