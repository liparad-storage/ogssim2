# CMake Header
cmake_minimum_required (VERSION 2.8)
project (OGSSim)

# Version Number
set (OGSSim_VERSION_MAJOR 2)
set (OGSSim_VERSION_MINOR 0)

# Modules
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ./)

# Includes
configure_file ("../include/config.h.in" "../include/config.h")

if (USE_MPI_BOOST)
	message (STATUS "MPI boost binding: on")
	set (EXTRA_LIBS ${EXTRA_LIBS} boost_mpi boost_serialization)
else ()
	message (STATUS "MPI boost binding: off")
endif (USE_MPI_BOOST)

# Libraries
if (USE_STATIC_LIBRARIES)
	message (STATUS "Static libraries (gflags/zmq): on")

	set (STATIC_LIBS ../libs/gflags-2.2.1/build/include ../libs/zeromq-4.1.6/include ../libs/cppzmq-4.3.0)

	add_library (gflags STATIC IMPORTED)
	set_target_properties (gflags PROPERTIES IMPORTED_LOCATION ../libs/libgflags.a)
	add_library (zmq STATIC IMPORTED)
	set_target_properties (zmq PROPERTIES IMPORTED_LOCATION ../libs/libzmq.a)

	set (EXTRA_LIBS ${EXTRA_LIBS})
else ()
	message (STATUS "Static libraries (gflags/zmq): off")
endif (USE_STATIC_LIBRARIES)

if (USE_STATIC_GLOG)
	message (STATUS "Static glog: on")

	set (STATIC_LIBS ${STATIC_LIBS} ../libs/glog-0.3.5/include)

	add_library (glog STATIC IMPORTED)
	set_target_properties (glog PROPERTIES IMPORTED_LOCATION ../libs/libglog.a)
else ()
	message (STATUS "Static glog: off")
endif (USE_STATIC_GLOG)

if (USE_TINYXML)
	message (STATUS "XML library: tinyxml2")
	set (EXTRA_LIBS ${EXTRA_LIBS} tinyxml2)
else ()
	message (STATUS "XML library: xerces-c")
	set (EXTRA_LIBS ${EXTRA_LIBS} xerces-c)
endif (USE_TINYXML)

set (EXTRA_LIBS ${EXTRA_LIBS} glog gflags zmq pthread)

if (USE_PYTHON_BINDING)
	message (STATUS "Python binding: on")
	find_package (PythonLibs)
	set (PYTHON_INC ${PYTHON_INCLUDE_DIRS})
	set (EXTRA_LIBS ${EXTRA_LIBS} python3.6m)
else ()
	message (STATUS "Python binding: off")
endif (USE_PYTHON_BINDING)

include_directories (../include ../libs ${STATIC_LIBS} ${PYTHON_INC})

# Sources
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ../)
file (GLOB SRC communication/*.cpp computation/*.cpp controller/*.cpp driver/*.cpp driver/maintenance/*.cpp graph/*.cpp module/*.cpp parser/*.cpp scheme/*.cpp serializer/*.cpp structure/*.cpp synchronization/*.cpp util/*.cpp *.cpp controller/utest/ut_*.cpp scheme/utest/ut_*.cpp synchronization/utest/ut_*.cpp)

# Definitions
set (CMAKE_CXX_FLAGS_UTEST ${CMAKE_CXX_FLAGS} "-std=c++14 -O0 -g -fsanitize=address -fno-omit-frame-pointer -DUTEST")
#set (CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS} "-std=c++14 -O0 -g -fno-omit-frame-pointer -D_GLIBCXX_DEBUG")
set (CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS} "-std=c++14 -O0 -g -fno-omit-frame-pointer")
#set (CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS} "-std=c++14 -O0 -g -fsanitize=address -fno-omit-frame-pointer")
set (CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS} "-std=c++14 -O3 -DNDEBUG")
set (CMAKE_CXX_FLAGS_MPIDBG ${CMAKE_CXX_FLAGS} "-std=c++14 -O0 -DOGSSMPI")
set (CMAKE_CXX_FLAGS_MPI ${CMAKE_CXX_FLAGS} "-std=c++14 -O3 -DNDEBUG -DOGSSMPI")

# Executable
add_executable (OGSSim ${SRC})
target_link_libraries (OGSSim ${EXTRA_LIBS})
