set(PROJECT_NAME xline_drawer)
project(${PROJECT_NAME})

file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.hpp)

# TODO convert to pure XObject and SHARED library to allow linking
x_add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

x_target_link_libraries(${PROJECT_NAME} xcs xobject)

find_package(UrbiAll)
use_urbi(${PROJECT_NAME})

# opencv
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS} )

# installation
install(TARGETS ${PROJECT_NAME}
        DESTINATION lib/xcs/xobjects)