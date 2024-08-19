# Include FMOD headers
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Link FMOD libraries
add_custom_target(copy_fmodDLL ALL)
add_custom_command(
	TARGET copy_fmodDLL POST_BUILD
	COMMAND ${CMAKE_COMMAND}
	-E copy
	${CMAKE_CURRENT_SOURCE_DIR}/dll/debug/fmodL64.dll
	$<TARGET_FILE_DIR:${EXECUTABLE_NAME}>
)

target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/debug/fmodL64_vc.lib)