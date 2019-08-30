# CMake Header
cmake_minimum_required (VERSION 2.8)
project (OGMDSim)

# Version Number
set (OGMDSim_VERSION_MAJOR 0)
set (OGMDSim_VERSION_MINOR 1)

# Modules
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ./)

# Libraries
set (EXTRA_LIBS ${EXTRA_LIBS} glog gflags tinyxml2)
include_directories (../include)

# Sources
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ../)
file (GLOB SRC *.cpp)

# Definitions
set (CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS} "-std=c++14 -O0 -g -fno-omit-frame-pointer")
set (CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS} "-std=c++14 -O3 -DNDEBUG")

# Executable
add_executable (OGMDSim ${SRC})
target_link_libraries (OGMDSim ${EXTRA_LIBS})
