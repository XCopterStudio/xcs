#
# Platform dispatch helpers
# Here is detected supported platform and set variables:
#     X_WIN (general Windows)
#     X_LIN  (general Linux)
#

if(WIN32)
    set(X_WIN true)
    set(X_LIN false)
elseif(UNIX)
    set(X_WIN false)
    set(X_LIN true)
endif()

if(X_WIN)
    set(TPN_WIN32 "/EHsc")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
    set(CMAKE_MODULE_LINKER_FLAGS "/SAFESEH:NO")
    add_definitions(-D_USE_MATH_DEFINES) # workaround for broken cmath in libport + xcs
    add_definitions(-D_WIN32_WINNT=0x0501)
    set(PATH_SEPARATOR ";")
    set(OpenCV_DIR "${OPENCV_ROOT}")
    set(VREP_REMOTE_ROOT ${VREP_REMOTE_ROOT}/windows)
    set(THREADS_USE_PTHREADS_WIN32 true)
    set(PTHREADS_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../3rd-party/pthreads)
elseif(X_LIN)
    set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined") # show unresolved symbols during link time
    set(CMAKE_MODULE_LINKER_FLAGS "-Wl,--no-undefined") # show unresolved symbols during link time
    set(CMAKE_CXX_FLAGS "-std=c++11 -Wreorder") # initializers list match fields declaration
    set(PATH_SEPARATOR ":")
    set(OpenCV_DIR "${OPENCV_ROOT}/share/OpenCV")
    set(VREP_REMOTE_ROOT ${VREP_REMOTE_ROOT}/linux)
endif()
