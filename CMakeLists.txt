cmake_minimum_required (VERSION 3.22)
Project (ScannerClient VERSION 0.01)

include("${CMAKE_CURRENT_SOURCE_DIR}/src/cmake/macros.cmake")

#default to Release build
#if(NOT CMAKE_BUILD_TYPE)
#	set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
#endif()


#print_environment_info()
#print_compiler_info()

IF (WIN32)
    #set(wxWidgets_USE_STATIC ON)

	# compile in support for both direct sound and WASAPI
    set(USE_AUDIO_DS ON CACHE BOOL "Include support for DirectSound")
    set(USE_AUDIO_WASAPI ON CACHE BOOL "Include support for WASAPI Audio")

    # WASAPI
    IF(USE_AUDIO_WASAPI)
        ADD_DEFINITIONS(-D__WINDOWS_WASAPI__)    
        IF (NOT MSVC)    
            SET(OTHER_LIBRARIES ${OTHER_LIBRARIES} -luuid -lksuser)
        ENDIF(NOT MSVC)
    ENDIF(USE_AUDIO_WASAPI)

    # DirectSound
    IF (USE_AUDIO_DS)    
        ADD_DEFINITIONS(-D__WINDOWS_DS__)    
        IF (MSVC)    
            SET(OTHER_LIBRARIES ${OTHER_LIBRARIES} dsound.lib)
        ELSE (MSVC)
            SET(OTHER_LIBRARIES ${OTHER_LIBRARIES} -ldsound)
        ENDIF (MSVC)
    ENDIF(USE_AUDIO_DS)    
  
ENDIF (WIN32)



# scannerclient library
set( sc_sources
	${sc_sources}
	"${PROJECT_SOURCE_DIR}/src/network/UDPSocket.cpp"
	"${PROJECT_SOURCE_DIR}/src/network/TCPSocket.cpp"
	"${PROJECT_SOURCE_DIR}/src/network/Socket.cpp"
	"${PROJECT_SOURCE_DIR}/src/network/RTSPRequest.cpp"
	"${PROJECT_SOURCE_DIR}/src/network/RTSPResponse.cpp"
	"${PROJECT_SOURCE_DIR}/src/network/SessionDescription.cpp"
	"${PROJECT_SOURCE_DIR}/src/network/MediaDescription.cpp"
	"${PROJECT_SOURCE_DIR}/src/network/RTSPSession.cpp"
	"${PROJECT_SOURCE_DIR}/src/network/RTPSession.cpp"
	"${PROJECT_SOURCE_DIR}/src/network/g711.cpp"
	"${PROJECT_SOURCE_DIR}/src/utils/utils.cpp"
	"${PROJECT_SOURCE_DIR}/src/audio/AudioBuffer.cpp"
	"${PROJECT_SOURCE_DIR}/src/audio/SC_RTaudio.cpp"
	"${PROJECT_SOURCE_DIR}/src/status/ScannerStatus.cpp"



)

add_library(sc STATIC ${sc_sources})
target_include_directories(sc
							PUBLIC  ${PROJECT_SOURCE_DIR}/src/include
							PRIVATE ${PROJECT_SOURCE_DIR}/src
							PRIVATE ${PROJECT_SOURCE_DIR}/external
							PRIVATE ${PROJECT_SOURCE_DIR}/external/fmt/include
							PRIVATE ${PROJECT_SOURCE_DIR}/external/spdlog/include
							)
set_target_properties(sc PROPERTIES CXX_STANDARD 20)
set_target_properties(sc PROPERTIES CXX_EXTENSIONS OFF)
set_target_properties(sc PROPERTIES CXX_STANDARD_REQUIRED ON)
set_target_properties(sc PROPERTIES POSITION_INDEPENDENT_CODE 1)

# rtsp_test executable
add_executable (rtsp_test
				"${PROJECT_SOURCE_DIR}/src/tests/rtsp_test.cpp"
				"${PROJECT_SOURCE_DIR}/external/tinyxml2/tinyxml2.cpp"
				"${PROJECT_SOURCE_DIR}/external/rtaudio/RtAudio.cpp"
				
)
target_include_directories(rtsp_test 
						PUBLIC  ${PROJECT_SOURCE_DIR}/src/include
						PRIVATE ${PROJECT_SOURCE_DIR}/src
						PRIVATE ${PROJECT_SOURCE_DIR}/src/tests
						PRIVATE ${PROJECT_SOURCE_DIR}/external/fmt/include
						PRIVATE ${PROJECT_SOURCE_DIR}/external/spdlog/include
						PRIVATE ${PROJECT_SOURCE_DIR}/external
						)
set_target_properties(rtsp_test PROPERTIES CXX_STANDARD 20)
set_target_properties(rtsp_test PROPERTIES CXX_EXTENSIONS OFF)
set_target_properties(rtsp_test PROPERTIES CXX_STANDARD_REQUIRED ON)
set_target_properties(rtsp_test PROPERTIES POSITION_INDEPENDENT_CODE 1)
target_link_libraries(rtsp_test sc ${OTHER_LIBRARIES})
