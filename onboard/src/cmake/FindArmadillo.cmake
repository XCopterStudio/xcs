#
# Armadillo library dependency
#

# Paths
set(CMAKE_LIBRARY_PATH ${BLAS_ROOT};${LAPACK_ROOT};${CMAKE_LIBRARY_PATH})

set(Uobject_DIR ${URBI_ROOT}/share/cmake/uobject)
find_package(Uobject)
mark_as_advanced(UOBJECT_LIBRARIES)


find_library(BLAS_LIBRARIES blas NAMES blas_win64_MT)
message(${BLAS_LIBRARIES})
set(BLAS_INCLUDE_DIRS )

find_library(LAPACK_LIBRARIES lapack NAMES lapack_win64_MT)
message(${LAPACK_LIBRARIES})
set(LAPACK_INCLUDE_DIRS )

set(ARMADILLO_INCLUDE_DIRS ${armadillo_INCLUDE_DIRS};${BLAS_ROOT}/include;${LAPACK_ROOT}/include)
set(ARMADILLO_LIBRARIES ${BLAS_LIBRARIES}; ${LAPACK_LIBRARIES})


function(use_armadillo TARGET_NAME)
include_directories(${ARMADILLO_INCLUDE_DIRS})
target_link_libraries(${TARGET_NAME} ${ARMADILLO_LIBRARIES})
endfunction(use_armadillo)