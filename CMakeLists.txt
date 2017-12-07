cmake_minimum_required(VERSION 2.6)

project(NBodyGravity)

# Use C++11
set(CMAKE_CXX_STANDARD 11)

add_subdirectory(CefiedGame)
include_directories(CefiedGame)

set(GAME_SOURCE_FILES
	App.cpp
	App.h
	DefaultScene.cpp
	DefaultScene.h
	NBodyGravity.cpp
	NBodyGravity.h
	CameraController.cpp
	CameraController.h
)

set(GAME_SHADER_FILES
	Shaders/_blendStates.fx
	Shaders/_rasterizerStates.fx
	Shaders/_depthStencilStates.fx
	Shaders/_samplerStates.fx

	Shaders/NBodyGravity.fx
)

set(GAME_MEDIA_FILES 
	Media/particle.dds
)
source_group("Shader Files" FILES ${GAME_SHADER_FILES})
source_group("Media Files" FILES ${GAME_MEDIA_FILES})

# Setup VDBGame build
add_executable(NBodyGravity ${GAME_SOURCE_FILES} ${GAME_SHADER_FILES} ${GAME_MEDIA_FILES})
target_link_libraries(NBodyGravity CefiedGame)
target_link_libraries(NBodyGravity debug ${FX11_DEBUG_LIBRARIES})
target_link_libraries(NBodyGravity optimized ${FX11_LIBRARIES})
include_directories(${FX11_INCLUDE_DIRS})


# Run post build copy for shaders
add_custom_command(
        TARGET NBodyGravity POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory copy_if_different
                ${CMAKE_SOURCE_DIR}/Shaders
                ${CMAKE_CURRENT_BINARY_DIR}/Shaders)

add_custom_command(
        TARGET NBodyGravity POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory copy_if_different
                ${CMAKE_SOURCE_DIR}/Media
                ${CMAKE_CURRENT_BINARY_DIR}/Media)