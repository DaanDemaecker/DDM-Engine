add_library(imgui_library STATIC
	"include/imgui/imgui.cpp"
	"include/imgui/imgui_draw.cpp"
	"include/imgui/imgui_widgets.cpp"
	"include/imgui/imgui_tables.cpp"
	"include/imgui/imgui_demo.cpp"
	"include/imgui/imgui_plot.cpp"
	"include/imgui/backends/imgui_impl_glfw.cpp"
	"include/imgui/backends/imgui_impl_vulkan.cpp"
)

# Find the required packages
find_package(Vulkan REQUIRED)

# Link Vulkan library
target_link_libraries(imgui_library PRIVATE Vulkan::Vulkan)

target_link_libraries(imgui_library PRIVATE glfw)
target_include_directories(imgui_library INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/imgui/backends)
target_include_directories(imgui_library INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/imgui)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE imgui_library)