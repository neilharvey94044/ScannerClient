cmake_minimum_required (VERSION 3.22)
Project (ScannerClient VERSION 0.01)

include("${CMAKE_CURRENT_SOURCE_DIR}/src/cmake/macros.cmake")

# Link Time Optimization
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported)
if(ipo_supported)
 	set(CMAKE_INTERPROCEDURAL_OPTIMIZATION True)
endif()

#default to Release build
#if(NOT CMAKE_BUILD_TYPE)
#	set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
#endif()

print_environment_info()
print_compiler_info()

set( scannerclient_sources
	${scannerclient_sources}
	src/network/UDPSocket.cpp
	src/network/TCPSocket.cpp
	src/network/Socket.cpp

)


add_executable (tcp_client
	${PROJECT_SOURCE_DIR}/src/tests/tcp_client.cpp
	${scannerclient_sources}
)

set_target_properties(tcp_client PROPERTIES CXX_STANDARD 17)
set_target_properties(tcp_client PROPERTIES CXX_EXTENSIONS OFF)
set_target_properties(tcp_client PROPERTIES CXX_STANDARD_REQUIRED ON)
set_target_properties(tcp_client PROPERTIES POSITION_INDEPENDENT_CODE 1)

target_include_directories(tcp_client PRIVATE
	src
	src/audio
	src/cli
	src/control
	src/gui
	src/network
	src/tests
	external/spdlog/include
	
	)
add_executable (udp_client
	${PROJECT_SOURCE_DIR}/src/tests/udp_client.cpp
	${scannerclient_sources}
)

set_target_properties(udp_client PROPERTIES CXX_STANDARD 17)
set_target_properties(udp_client PROPERTIES CXX_EXTENSIONS OFF)
set_target_properties(udp_client PROPERTIES CXX_STANDARD_REQUIRED ON)
set_target_properties(udp_client PROPERTIES POSITION_INDEPENDENT_CODE 1)

target_include_directories(udp_client PRIVATE
	src
	src/audio
	src/cli
	src/control
	src/gui
	src/network
	src/tests
	external/spdlog/include
	
	)
