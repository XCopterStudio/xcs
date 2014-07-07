set(PROJECT_NAME xsettings)
project(${PROJECT_NAME})

file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.hpp)
file(GLOB XSETTINGS ../../../../xsetting/*.xs)

add_library(${PROJECT_NAME} MODULE ${SOURCES} ${HEADERS} ${XSETTINGS})

x_target_link_libraries(${PROJECT_NAME} xcs)
x_target_link_libraries(${PROJECT_NAME} xobject)

find_package(UrbiAll)
use_urbi(${PROJECT_NAME})

add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_SOURCE_DIR}/../xsetting/ $<TARGET_FILE_DIR:${PROJECT_NAME}>/xsetting)