set(PROJECT_NAME xlocalization)
project(${PROJECT_NAME})

file(GLOB SOURCES *.cpp tum/*.cpp)
file(GLOB HEADERS *.hpp tum/*.h tum/*.hpp)

if(X_WIN)
    include_directories(${PTAM_ROOT}/include ${PTHREADS_ROOT}/include)
elseif(X_LIN)
    include_directories(${PTAM_ROOT}/include)
endif()

add_x_object(${PROJECT_NAME} ${SOURCES} ${HEADERS})

find_package(Boost REQUIRED COMPONENTS thread)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})

find_package(OpenGL)
find_package(GLUT)

find_library(PTAM_LIB xcs-ptam PATHS ${PTAM_ROOT}/lib)
find_library(GVARS_LIB GVars3 PATHS ${PTAM_ROOT}/lib)
find_library(CVD_LIB cvd PATHS ${PTAM_ROOT}/lib)
set(PTAM_LIBRARIES ${PTAM_LIB} ${GVARS_LIB} ${CVD_LIB})

if(X_WIN)
    find_library (PTREADS_LIB pthreads NAMES pthreadVC2 PATHS ${PTHREADS_ROOT}/lib/x64)
    target_link_libraries(${PROJECT_NAME} ${PTAM_LIBRARIES} ${OPENGL_LIBRARY} ${GLUT_LIBRARY} ${PTREADS_LIB})
elseif(X_LIN)
    target_link_libraries(${PROJECT_NAME} ${PTAM_LIBRARIES} ${OPENGL_LIBRARY} ${GLUT_LIBRARY})
endif()

# Must be after PTAM library.
find_package(Armadillo)
use_armadillo(${PROJECT_NAME})

use_logging(${PROJECT_NAME})