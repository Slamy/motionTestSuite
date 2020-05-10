
cmake_minimum_required (VERSION 2.8.11)
project (MotionTestSuite CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

add_executable (strobeCrossTalk strobeCrossTalk.cpp)
target_link_libraries (strobeCrossTalk
	SDL2
	GL
	GLU
	)
