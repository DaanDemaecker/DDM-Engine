# Set source and destination root directories
set(SHADER_SOURCE_DIR "${CMAKE_SOURCE_DIR}/resources")
set(SHADER_BINARY_DIR "${CMAKE_BINARY_DIR}/${SOURCE_FOLDER_NAME}/resources")

# Recursively find all shader files
file(GLOB_RECURSE GLSL_SOURCE_FILES
    CONFIGURE_DEPENDS
    "${SHADER_SOURCE_DIR}/**/*.vert"
    "${SHADER_SOURCE_DIR}/**/*.frag"
)

set(SPIRV_BINARY_FILES "")

foreach(GLSL ${GLSL_SOURCE_FILES})
    # Get the relative path of the file (e.g., shaders/basic.vert -> shaders/basic.vert)
    file(RELATIVE_PATH REL_PATH "${SHADER_SOURCE_DIR}" "${GLSL}")

    # Change output extension to .spv
    set(SPIRV "${SHADER_BINARY_DIR}/${REL_PATH}.spv")

    # Ensure the output directory exists
    get_filename_component(SPIRV_DIR "${SPIRV}" DIRECTORY)
    file(MAKE_DIRECTORY "${SPIRV_DIR}")

    # Add a command to compile the shader
    add_custom_command(
        OUTPUT "${SPIRV}"
        COMMAND ${Vulkan_GLSLC_EXECUTABLE} -g "${GLSL}" -o "${SPIRV}"
        DEPENDS "${GLSL}"
        COMMENT "Compiling shader ${REL_PATH}"
    )

    # Collect all outputs
    list(APPEND SPIRV_BINARY_FILES "${SPIRV}")
endforeach()


message(STATUS "Finished compiling shaders")

# Create a custom target that depends on all compiled shaders
add_custom_target(Shaders ALL
    DEPENDS ${SPIRV_BINARY_FILES}
)

# Make your main executable depend on Shaders
add_dependencies(${EXECUTABLE_NAME} Shaders)
