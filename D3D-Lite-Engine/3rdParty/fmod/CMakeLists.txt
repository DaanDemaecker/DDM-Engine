# Include FMOD headers
target_include_directories(D3D-Lite-Engine_Build PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Link FMOD libraries
add_custom_target(copy_fmodDLL ALL)
add_custom_command(
	TARGET copy_fmodDLL POST_BUILD
	COMMAND ${CMAKE_COMMAND}
	-E copy
	${CMAKE_CURRENT_SOURCE_DIR}/dll/debug/fmodL64.dll
	$<TARGET_FILE_DIR:D3D-Lite-Engine_Build>
)

target_link_libraries(D3D-Lite-Engine_Build PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/debug/fmodL64_vc.lib)