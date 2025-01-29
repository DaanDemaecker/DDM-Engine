
target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/debug/libfbxsdk.lib)

# Include FBX SDK headers
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Link FBX SDK libraries
target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/debug/libfbxsdk.lib)



add_custom_target(copy_fbxsdkDLL ALL)
# Copy the FBX SDK DLL to the output directory after build
add_custom_command(
    TARGET copy_fbxsdkDLL POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/dll/debug/libfbxsdk.dll
    $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>
)

# Optional: Specify properties for MSVC runtime library linkage
if (MSVC)
    set_property(TARGET ${EXECUTABLE_NAME} PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()