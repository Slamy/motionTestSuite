
cmake_minimum_required (VERSION 2.8.11)
project (MotionTestSuite CXX)

set(Boost_USE_STATIC_LIBS        ON)  # only find static libs
set(Boost_USE_DEBUG_LIBS         OFF) # ignore debug libs and
set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
find_package(Boost 1.66.0 COMPONENTS program_options)

if(Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

	add_executable (motionTestSuite motionTestSuite.cpp)
	target_link_libraries (motionTestSuite
		SDL2
		GL
		GLU
		${Boost_LIBRARIES}
		)

endif()
