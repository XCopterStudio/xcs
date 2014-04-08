set(PROJECT_NAME ulinedrawer)
project(${PROJECT_NAME})

file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.hpp)

x_add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

x_target_link_libraries(${PROJECT_NAME} xcs)

#if(X_LIN)
#add_definitions(-rdynamic)
#endif()

find_package(UrbiAll)
use_urbi(${PROJECT_NAME})

# opencv
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS} )
