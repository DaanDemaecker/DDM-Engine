# Include FMOD headers
target_include_directories(D3D-Lite-Engine_Build PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Link FMOD libraries
add_custom_target(copy_fbxsdkDLL ALL)
add_custom_command(
	TARGET copy_fbxsdkDLL POST_BUILD
	COMMAND ${CMAKE_COMMAND}
	-E copy
	${CMAKE_CURRENT_SOURCE_DIR}/bin/debug/libfbxsdk.dll
	$<TARGET_FILE_DIR:D3D-Lite-Engine_Build>
)

target_link_libraries(D3D-Lite-Engine_Build PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/debug/libfbxsdk.lib)