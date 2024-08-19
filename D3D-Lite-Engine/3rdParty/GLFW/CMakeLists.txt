include(FetchContent)

# Declare GLFW as a dependency
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        3.3.8 # Specify the version you need
  GIT_SHALLOW TRUE
)

# Set GLFW options before making it available
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
set(GLFW_DOCUMENT_INTERNALS OFF)

# Make GLFW available after setting options
FetchContent_MakeAvailable(glfw)

# Link GLFW to your executable
target_link_libraries(${EXECUTABLE_NAME} PRIVATE glfw)
