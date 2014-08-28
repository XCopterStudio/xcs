set(PROJECT_NAME xobject)
project(${PROJECT_NAME})

file(GLOB SOURCES *.cpp ../../types/*.cpp)
file(GLOB HEADERS *.hpp)
file(GLOB CHEADERS *.h)
file(GLOB SCRIPTS ../../../../urbiscript/onboard/*.u)
file(GLOB INC_SCRIPTS ../../../../urbiscript/onboard/inc/*.u)
file(GLOB NODE_SCRIPTS ../../../../urbiscript/onboard/nodes/*.u)

x_add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS} ${CHEADERS} ${SCRIPTS} ${INC_SCRIPTS} ${NODE_SCRIPTS} ${BOOT_IGNORE})

x_target_link_libraries(${PROJECT_NAME} xcs)

find_package(UrbiAll)
use_urbi(${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}
        DESTINATION lib)

install(FILES ${HEADERS} DESTINATION include/xcs/nodes/xobject)


