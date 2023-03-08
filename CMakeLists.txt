# Copyright (c) Neil D. Harvey
# SPDX-License-Identifier: GPL-2.0+

cmake_minimum_required (VERSION 3.25)
SET(CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE} CACHE STRING "" FORCE)

Project (ScannerClient VERSION 0.01)

include("${CMAKE_CURRENT_SOURCE_DIR}/src/cmake/macros.cmake")

#default to Release build
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()


print_environment_info()
print_compiler_info()

#---------------------- rtAudio --------------------------

IF (WIN32)

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

IF(UNIX AND NOT APPLE)
	SET(USE_AUDIO_PULSE ON CACHE BOOL "Use Pulse Audio")
	SET(USE_AUDIO_ALSA OFF CACHE BOOL "Use Alsa Audio")
	SET(USE_AUDIO_JACK OFF CACHE BOOL "Use Jack Audio")
	SET(USE_AUDIO_OSS OFF CACHE BOOL "Use OSS Audio")

	IF(USE_AUDIO_PULSE)
	SET (OTHER_LIBRARIES ${OTHER_LIBRARIES} pulse-simple pulse)
	ADD_DEFINITIONS(
		 -D__LINUX_PULSE__
	)
 	ENDIF(USE_AUDIO_PULSE)

	IF(USE_AUDIO_JACK)
	 	find_package(Jack)
	 	SET (OTHER_LIBRARIES ${OTHER_LIBRARIES} ${JACK_LIBRARIES})
	 	ADD_DEFINITIONS(
		  -D__UNIX_JACK__
	 	)
	 	include_directories(${JACK_INCLUDE_DIRS})
  	ENDIF(USE_AUDIO_JACK)

	IF(USE_AUDIO_ALSA)
	  	SET (OTHER_LIBRARIES ${OTHER_LIBRARIES} asound)
	  	set(ALSA_INCLUDE_DIR "/usr/include" CACHE FILEPATH "ALSA include path")
	  	include_directories(${ALSA_INCLUDE_DIR})
	  	set(ALSA_LIB_DIR "/usr/lib" CACHE FILEPATH "ALSA lib path")
	  	link_directories(${ALSA_LIB_DIR})
	  	ADD_DEFINITIONS(
		  -D__LINUX_ALSA__
	  	)
   ENDIF(USE_AUDIO_ALSA)

   	IF(USE_AUDIO_OSS)
   		SET (OTHER_LIBRARIES ${OTHER_LIBRARIES} oss)
   		ADD_DEFINITIONS(
		-D__LINUX_OSS__
   		)	
	ENDIF(USE_AUDIO_OSS)



ENDIF(UNIX AND NOT APPLE)


IF (APPLE)
    SET(CMAKE_OSX_DEPLOYMENT_TARGET, "10.15")
  
    link_directories(/usr/local/lib)
    link_directories(/opt/local/lib)

    ADD_DEFINITIONS(
      -D__MACOSX_CORE__
    )    

   FIND_LIBRARY(COREAUDIO_LIBRARY CoreAudio)
   SET (OTHER_LIBRARIES ${COREAUDIO_LIBRARY} ${OTHER_LIBRARIES})
   set(BUNDLE_APP OFF CACHE BOOL "Bundle Application")

ENDIF (APPLE)

#--------------------- libsamplerate -----------------------
add_subdirectory(external/libsamplerate)


 #-------------------- wxWidgets ----------------------------
 # Create command line variables or use the settings below to help "find_package".
 # The key is to both build and install wxWidgets to the same directory.
 # For Linux there is also a bug in the FindwxWidgets.cmake script provided by
 # cmake.  To overcome this bug we set a WX_CONFIG environment variable to
 # point to the wx-config executable in our wxWidgets install directory. WX_CONFIG
 # is not a cmake variable, it's an environment variable.  See the defaults below.
 # For Windows make sure the wxWidgets_ROOT_DIR points to the build/install directory.
 # Note that the Windows drive specified is "D:", yours might be "C:".
 IF(WIN32)
	IF(NOT wxWidgets_ROOT_DIR)
		SET(wxWidgets_ROOT_DIR D:/dev/repos/wxWidgets-3.2.1)
	ENDIF(NOT wxWidgets_ROOT_DIR)
	SET(wxWidgets_CONFIGURATION mswu)
	#SET(wxWidgets_CONFIGURATION mswud)
ELSE()
	IF(NOT wxWidgets_ROOT_DIR)
		SET(wxWidgets_ROOT_DIR ~/wx_install)
	ENDIF(NOT wxWidgets_ROOT_DIR)
	IF(NOT WX_CONFIG)
		SET(ENV{WX_CONFIG} ~/wx_install/bin/wx-config)
	ENDIF(NOT WX_CONFIG)
ENDIF (WIN32)

IF(APPLE)
	IF(NOT WX_CONFIG)
		SET(ENV{WX_CONFIG} ~/wx_install/wx-config)
	ENDIF(NOT WX_CONFIG)
ENDIF(APPLE)

# Request the required wxWidgets libs
find_package(wxWidgets REQUIRED base core propgrid)

# Include the wxWidgets use file to initialize various settings
if(wxWidgets_USE_FILE)
    include(${wxWidgets_USE_FILE})
endif()


#--------------------- The Library --------------------
set( scl_sources
	${sc_sources}
	${PROJECT_SOURCE_DIR}/src/network/UDPSocket.cpp
	${PROJECT_SOURCE_DIR}/src/network/TCPSocket.cpp
	${PROJECT_SOURCE_DIR}/src/network/Socket.cpp
	${PROJECT_SOURCE_DIR}/src/network/RTSPRequest.cpp
	${PROJECT_SOURCE_DIR}/src/network/RTSPResponse.cpp
	${PROJECT_SOURCE_DIR}/src/network/SessionDescription.cpp
	${PROJECT_SOURCE_DIR}/src/network/MediaDescription.cpp
	${PROJECT_SOURCE_DIR}/src/network/RTSPSession.cpp
	${PROJECT_SOURCE_DIR}/src/network/RTPSession.cpp
	${PROJECT_SOURCE_DIR}/src/network/g711.cpp
	${PROJECT_SOURCE_DIR}/src/utils/utils.cpp
	${PROJECT_SOURCE_DIR}/src/audio/AudioBuffer.cpp
	${PROJECT_SOURCE_DIR}/src/audio/SC_RTaudio.cpp
	${PROJECT_SOURCE_DIR}/src/status/ScannerStatus.cpp
	${PROJECT_SOURCE_DIR}/src/config/SC_CONFIG.cpp
	${PROJECT_SOURCE_DIR}/src/config/WX_ConfigImpl.cpp
	${PROJECT_SOURCE_DIR}/src/control/SControl.cpp
)

add_library(scl STATIC ${scl_sources})
target_include_directories(scl
							PRIVATE ${PROJECT_SOURCE_DIR}/src
							PRIVATE ${PROJECT_SOURCE_DIR}/external
							PRIVATE ${PROJECT_SOURCE_DIR}/external/fmt/include
							PRIVATE ${PROJECT_SOURCE_DIR}/external/spdlog/include
							PRIVATE ${PROJECT_SOURCE_DIR}/external/libsamplerate/include
							)
set_target_properties(scl PROPERTIES CXX_STANDARD 20)
set_target_properties(scl PROPERTIES CXX_EXTENSIONS OFF)
set_target_properties(scl PROPERTIES CXX_STANDARD_REQUIRED ON)
set_target_properties(scl PROPERTIES POSITION_INDEPENDENT_CODE 1)

#--------------- The Test Executable --------------------------
add_executable (sc_test
				${PROJECT_SOURCE_DIR}/src/tests/sc_test.cpp
				${PROJECT_SOURCE_DIR}/external/tinyxml2/tinyxml2.cpp
				${PROJECT_SOURCE_DIR}/external/rtaudio/RtAudio.cpp
				
)
target_include_directories(sc_test 
						PRIVATE ${PROJECT_SOURCE_DIR}/src
						PRIVATE ${PROJECT_SOURCE_DIR}/src/tests
						PRIVATE ${PROJECT_SOURCE_DIR}/external/fmt/include
						PRIVATE ${PROJECT_SOURCE_DIR}/external/spdlog/include
						PRIVATE ${PROJECT_SOURCE_DIR}/external
						)
set_target_properties(sc_test PROPERTIES CXX_STANDARD 20)
set_target_properties(sc_test PROPERTIES CXX_EXTENSIONS OFF)
set_target_properties(sc_test PROPERTIES CXX_STANDARD_REQUIRED ON)
set_target_properties(sc_test PROPERTIES POSITION_INDEPENDENT_CODE 1)
target_link_libraries(sc_test PRIVATE scl ${wxWidgets_LIBRARIES} ${OTHER_LIBRARIES} )


#--------------- The GUI --------------------------

set(sc_sources 
	${sc_sources}
	${PROJECT_SOURCE_DIR}/src/gui/scApp.cpp
	${PROJECT_SOURCE_DIR}/src/gui/SCFrame.cpp
	${PROJECT_SOURCE_DIR}/src/gui/SettingsDialog.cpp
	${PROJECT_SOURCE_DIR}/external/rtaudio/RtAudio.cpp
	${PROJECT_SOURCE_DIR}/external/tinyxml2/tinyxml2.cpp
	
	)

if(WIN32)
    # Include a RC file for windows
    set(sc_sources ${sc_sources}
       ${PROJECT_SOURCE_DIR}/scannerclient.rc)
elseif(APPLE)
    # Add an icon for the apple .app file
    # list(APPEND sc_sources ../../src/osx/carbon/wxmac.icns)
endif()

if(APPLE)
    add_executable(sc MACOSX_BUNDLE ${sc_sources})
else()
# "WIN32" below causes winmain to be executed, but is ignored on other systems (e.g. Linux)
add_executable(sc WIN32 ${sc_sources})
endif()

target_include_directories(sc 
							PRIVATE ${PROJECT_SOURCE_DIR}/src
							PRIVATE ${PROJECT_SOURCE_DIR}/external
							PRIVATE ${PROJECT_SOURCE_DIR}/external/spdlog/include

)

set_target_properties(sc PROPERTIES CXX_STANDARD 20)
set_target_properties(sc PROPERTIES CXX_EXTENSIONS OFF)
set_target_properties(sc PROPERTIES CXX_STANDARD_REQUIRED ON)
set_target_properties(sc PROPERTIES POSITION_INDEPENDENT_CODE 1)
target_link_libraries(sc PRIVATE ${wxWidgets_LIBRARIES} samplerate scl ${OTHER_LIBRARIES})
