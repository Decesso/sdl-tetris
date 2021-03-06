cmake_minimum_required (VERSION 2.6)
project (Tetris)
include(CheckCXXCompilerFlag)
set(CMAKE_CXX_FLAGS "-g -Wall")
CHECK_CXX_COMPILER_FLAG("-std=c++14" COMPILER_SUPPORTS_CXX14)
if(COMPILER_SUPPORTS_CXX14)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
else()
	CHECK_CXX_COMPILER_FLAG("-std=c++1y" COMPILER_SUPPORTS_CXX1Y)
	if(COMPILER_SUPPORTS_CXX1Y)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")
	else()
		message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++14 support. Please use a different C++ compiler.")
	endif()
endif()

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/")
FIND_PACKAGE(CPPUNIT REQUIRED)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)
add_subdirectory(src)
add_subdirectory(test)

#clean
add_custom_target(clean-cmake-files
	COMMAND ${CMAKE_COMMAND} -P clean-all.cmake
)
add_custom_target(clean-all
	COMMAND ${CMAKE_BUILD_TOOL} clean
	COMMAND ${CMAKE_COMMAND} -P clean-all.cmake
)
