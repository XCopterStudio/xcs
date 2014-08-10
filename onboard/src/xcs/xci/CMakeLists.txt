set(PROJECT_NAME xci-dev)
project(${PROJECT_NAME})


file(GLOB HEADERS *.hpp)

install(FILES ${HEADERS} DESTINATION include/xcs/xci)

