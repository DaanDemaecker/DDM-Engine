# Find the required packages
find_package(Vulkan REQUIRED)

# Include Directories
include_directories(${Vulkan_INCLUDE_DIRS})

# Link Vulkan library
target_link_libraries(DDMEngine PUBLIC Vulkan::Vulkan)