set(PROJECT_NAME xptam)
project(${PROJECT_NAME})

file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.hpp)

include_directories(${PTAM_ROOT}/include)
find_library(PTAM_LIBRARIES ptam PATHS ${PTAM_ROOT}/lib)

add_library(${PROJECT_NAME} MODULE ${SOURCES} ${HEADERS})

x_target_link_libraries(${PROJECT_NAME} xobject)
target_link_libraries(${PROJECT_NAME} ${PTAM_LIBRARIES})

find_package(UrbiAll)
use_urbi(${PROJECT_NAME})

use_logging(${PROJECT_NAME})