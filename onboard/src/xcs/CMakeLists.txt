set(PROJECT_NAME xcs)
project(${PROJECT_NAME})

file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.hpp)

x_add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

if(X_LIN)
    target_link_libraries(${PROJECT_NAME} dl)
endif(X_LIN)

find_package(Boost REQUIRED COMPONENTS system filesystem)
include_directories(${Boost_INCLUDE_DIRS}) 
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})

use_logging(${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}
        DESTINATION lib)

install(FILES ${HEADERS} DESTINATION include/xcs)
install(FILES ${PROJECT_BINARY_DIR}/${PROJECT_NAME}_export.h DESTINATION include/xcs)

install(EXPORT ${PROJECT_NAME} DESTINATION share)