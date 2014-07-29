set(PROJECT_NAME xobject)
project(${PROJECT_NAME})

file(GLOB SOURCES *.cpp ../../types/*.cpp)
file(GLOB HEADERS *.hpp)
file(GLOB CHEADERS *.h)
file(GLOB SCRIPTS ../../../../urbiscript/*.u)
file(GLOB BOOT_SCRIPTS ../../../../urbiscript/boot/*.u)
file(GLOB BOOT_IGNORE ../../../../urbiscript/boot/.uignore)

x_add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS} ${CHEADERS} ${SCRIPTS} ${BOOT_SCRIPTS} ${BOOT_IGNORE})

x_target_link_libraries(${PROJECT_NAME} xcs)

find_package(UrbiAll)
use_urbi(${PROJECT_NAME})





