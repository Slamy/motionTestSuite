
cmake_minimum_required (VERSION 2.8.11)
project (MotionTestSuite CXX)

set(Boost_USE_STATIC_LIBS        ON)  # only find static libs
set(Boost_USE_DEBUG_LIBS         OFF) # ignore debug libs and
set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
find_package(Boost 1.66.0 COMPONENTS program_options)

if(Boost_FOUND)
	include_directories(
		${Boost_INCLUDE_DIRS}
		${CMAKE_CURRENT_SOURCE_DIR}/src
	)

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

	add_executable (motionTestSuite
		src/motionTestSuite.cpp
		src/tests/MotionTest.cpp
		src/tests/PursuitCamera.cpp
		src/tests/StrobeCrossTalk.cpp
		src/tests/VerticalText.cpp
		src/tests/GridPattern.cpp
		src/tests/SingleColor.cpp
		src/tests/Contrast.cpp
		)
		
	target_link_libraries (motionTestSuite
		SDL2
		GL
		GLU
		SDL2_ttf
		${Boost_LIBRARIES}
		)

endif()

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/testpattern.bmp
	${CMAKE_CURRENT_BINARY_DIR}/testpattern.bmp
	COPYONLY)

find_file(FONT
	NAMES
	FreeSans.ttf
	FreeSerif.ttf
	arial.ttf
	REQUIRED
	PATHS
	"/usr/share/fonts/truetype/freefont"
	"C:/Windows/Fonts/"
	)

configure_file(
	${FONT}
	${CMAKE_CURRENT_BINARY_DIR}/font.ttf
	COPYONLY)
	
