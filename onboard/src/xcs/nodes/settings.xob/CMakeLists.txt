set(PROJECT_NAME xsettings)
project(${PROJECT_NAME})

file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.hpp)
file(GLOB XSETTINGS ../../../../data/settings/*.xs)

add_x_object(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${XSETTINGS})

x_target_link_libraries(${PROJECT_NAME} xcs)
