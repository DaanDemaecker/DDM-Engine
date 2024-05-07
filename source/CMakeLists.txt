# Set cmake version
cmake_minimum_required(VERSION 3.11)

# Project name
project(D3D-Lite-Engine)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
cmake_policy(SET CMP0079 NEW)

add_subdirectory(D3D-Lite)
add_subdirectory(3rdParty)

add_custom_target(configFile ALL)
add_custom_command(
	TARGET configFile POST_BUILD
	COMMAND ${CMAKE_COMMAND}
	-E copy
	${CMAKE_SOURCE_DIR}/Config.json
	$<TARGET_FILE_DIR:D3D-Lite-Engine_Build>
)

add_custom_target(copy_resources ALL)
add_custom_command(
	TARGET copy_resources POST_BUILD
	COMMAND ${CMAKE_COMMAND}
	-E copy_directory
	${CMAKE_SOURCE_DIR}/resources
	$<TARGET_FILE_DIR:D3D-Lite-Engine_Build>/resources
)