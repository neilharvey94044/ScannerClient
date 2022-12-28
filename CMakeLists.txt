cmake_minimum_required (VERSION 3.22)
Project (ScannerClient VERSION 0.01)

include("${CMAKE_CURRENT_SOURCE_DIR}/src/cmake/macros.cmake")

# Boost does not implement the most recent format() specification, so using {fmt} instead.
# spdlog also exposes {fmt} but don't rely on it
# Leaving Boost here in case other features become useful
set(Boost_INCLUDE_DIR "d:/dev/repos/boost_1_81_0")
find_package(Boost)

# Link Time Optimization
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported)
if(ipo_supported)
 	set(CMAKE_INTERPROCEDURAL_OPTIMIZATION True)
endif()

#default to Release build
#if(NOT CMAKE_BUILD_TYPE)
#	set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
#endif()


#print_environment_info()
#print_compiler_info()


# network library
set( network_sources
	${network_sources}
	"${PROJECT_SOURCE_DIR}/src/network/UDPSocket.cpp"
	"${PROJECT_SOURCE_DIR}/src/network/TCPSocket.cpp"
	"${PROJECT_SOURCE_DIR}/src/network/Socket.cpp"
	"${PROJECT_SOURCE_DIR}/src/network/RTSPRequest.cpp"
	"${PROJECT_SOURCE_DIR}/src/network/RTSPResponse.cpp"
	"${PROJECT_SOURCE_DIR}/src/network/SessionDescription.cpp"
	"${PROJECT_SOURCE_DIR}/src/network/MediaDescription.cpp"
)

add_library(network STATIC ${network_sources})
target_include_directories(network
							PUBLIC  ${PROJECT_SOURCE_DIR}/src/include
							PRIVATE ${PROJECT_SOURCE_DIR}/src
							PRIVATE ${PROJECT_SOURCE_DIR}/external/fmt/include
							PRIVATE ${PROJECT_SOURCE_DIR}/external/spdlog/include
							)
set_target_properties(network PROPERTIES CXX_STANDARD 20)
set_target_properties(network PROPERTIES CXX_EXTENSIONS OFF)
set_target_properties(network PROPERTIES CXX_STANDARD_REQUIRED ON)
set_target_properties(network PROPERTIES POSITION_INDEPENDENT_CODE 1)

# rtsp_test executable
add_executable (rtsp_test
				"${PROJECT_SOURCE_DIR}/src/tests/rtsp_test.cpp"
				"${PROJECT_SOURCE_DIR}/external/tinyxml2/tinyxml2.cpp"
				
)
target_include_directories(rtsp_test 
						PUBLIC  ${PROJECT_SOURCE_DIR}/src/include
						PRIVATE ${PROJECT_SOURCE_DIR}/src
						PRIVATE ${PROJECT_SOURCE_DIR}/src/tests
						PRIVATE ${PROJECT_SOURCE_DIR}/external/format/include
						PRIVATE ${PROJECT_SOURCE_DIR}/external/spdlog/include
						PRIVATE ${PROJECT_SOURCE_DIR}/external
						)
set_target_properties(rtsp_test PROPERTIES CXX_STANDARD 20)
set_target_properties(rtsp_test PROPERTIES CXX_EXTENSIONS OFF)
set_target_properties(rtsp_test PROPERTIES CXX_STANDARD_REQUIRED ON)
set_target_properties(rtsp_test PROPERTIES POSITION_INDEPENDENT_CODE 1)
target_link_libraries(rtsp_test network)
