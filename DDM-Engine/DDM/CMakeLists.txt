# Create the executable
add_library(DDMEngine  SHARED

"DDM-Engine/BaseClasses/GameObject.cpp"

"DDM-Engine/EngineComponents/Camera.cpp"
"DDM-Engine/EngineComponents/Light/LightComponent.cpp"
"DDM-Engine/EngineComponents/MeshRenderer.cpp"
"DDM-Engine/EngineComponents/SkyBox.cpp"
"DDM-Engine/EngineComponents/Transform.cpp"
"DDM-Engine/EngineComponents/Audio/AudioSource.cpp"
"DDM-Engine/EngineComponents/Audio/AudioClip.cpp"
"DDM-Engine/EngineComponents/Audio/AudioListener.cpp"

"DDM-Engine/Events/Subject.cpp"

"DDM-Engine/DataTypes/DescriptorObjects/InputAttachmentDescriptorObject.cpp"
"DDM-Engine/DataTypes/DescriptorObjects/TextureDescriptorObject.cpp"

"DDM-Engine/DataTypes/Materials/Material.cpp"
"DDM-Engine/DataTypes/Materials/CubeMapMaterial.cpp"
"DDM-Engine/DataTypes/Materials/MultiMaterial.cpp"
"DDM-Engine/DataTypes/Materials/TexturedMaterial.cpp"

"DDM-Engine/Engine/DDMEngine.cpp"
"DDM-Engine/Engine/Scene.cpp"
"DDM-Engine/Engine/Window.cpp"
"DDM-Engine/Engine/DDMModelLoader.cpp"

"DDM-Engine/Managers/ConfigManager.cpp"
"DDM-Engine/Managers/SceneManager.cpp"
"DDM-Engine/Managers/TimeManager.cpp"
"DDM-Engine/Managers/ResourceManager.cpp"
"DDM-Engine/Managers/InputManager.cpp"
"DDM-Engine/Managers/Input/KeyboardImpl.cpp"
"DDM-Engine/Managers/Input/MouseImpl.h"
"DDM-Engine/Managers/Input/MouseImpl.cpp"
"DDM-Engine/Managers/Input/Mouse.cpp"

"DDM-Engine/Utils/Utils.cpp"

"DDM-Engine/Vulkan/VulkanObject.cpp"
"DDM-Engine/Vulkan/VulkanUtils.cpp"

"DDM-Engine/Vulkan/SpirVReflect/spirv_reflect.cpp"

"DDM-Engine/Vulkan/Renderers/ForwardRenderer.cpp"
"DDM-Engine/Vulkan/Renderers/DeferredRenderer.cpp"
"DDM-Engine/Vulkan/Renderers/AORenderers/SSAORenderer.cpp"
"DDM-Engine/Vulkan/Renderers/AORenderers/HBAORenderer.cpp"
"DDM-Engine/Vulkan/Renderers/AORenderers/GTAORenderer.cpp"

"DDM-Engine/Vulkan/ImGui/ImGuiWrapper.cpp"
"DDM-Engine/Vulkan/ImGui/ImGuiFunctions.cpp"

"DDM-Engine/Vulkan/VulkanManagers/BufferCreator.cpp"
"DDM-Engine/Vulkan/VulkanManagers/CommandpoolManager.cpp"
"DDM-Engine/Vulkan/VulkanManagers/ImageManager/ImageManager.cpp"
"DDM-Engine/Vulkan/VulkanManagers/ImageViewManager.cpp"
"DDM-Engine/Vulkan/VulkanManagers/PipelineManager.cpp"
"DDM-Engine/Vulkan/VulkanManagers/SyncObjectManager.cpp"
"DDM-Engine/Vulkan/VulkanManagers/ImageManager/STBImage.cpp"

"DDM-Engine/Vulkan/VulkanWrappers/DescriptorPoolWrapper.cpp"
"DDM-Engine/Vulkan/VulkanWrappers/GPUObject.cpp"
"DDM-Engine/Vulkan/VulkanWrappers/InstanceWrapper.cpp"
"DDM-Engine/Vulkan/VulkanWrappers/PipelineWrapper.cpp"
"DDM-Engine/Vulkan/VulkanWrappers/RenderpassWrapper.cpp"
"DDM-Engine/Vulkan/VulkanWrappers/ShaderModuleWrapper.cpp"
"DDM-Engine/Vulkan/VulkanWrappers/SurfaceWrapper.cpp" 
"DDM-Engine/Vulkan/VulkanWrappers/SwapchainWrapper.cpp"
"DDM-Engine/Vulkan/VulkanWrappers/VulkanCore.cpp"
"DDM-Engine/Vulkan/VulkanWrappers/Mesh.cpp"
"DDM-Engine/Vulkan/VulkanWrappers/QueryPool.cpp"
"DDM-Engine/Vulkan/VulkanWrappers/Attachment.cpp"
"DDM-Engine/Vulkan/VulkanWrappers/FrameBuffer.cpp"
"DDM-Engine/Vulkan/VulkanWrappers/Subpass.cpp"
"DDM-Engine/Vulkan/VulkanWrappers/Image.cpp"
"DDM-Engine/Vulkan/VulkanWrappers/Buffer.cpp"

"DDM-Engine/ServiceLocator/ServiceLocator.cpp"

"DDM-Engine/ServiceLocator/SoundSystem/Fmod/FmodSoundSystem.cpp"
"DDM-Engine/ServiceLocator/SoundSystem/Fmod/FmodSystem.cpp"
"DDM-Engine/ServiceLocator/SoundSystem/Fmod/FmodChannel.cpp"

"DDM-Engine/ServiceLocator/FileSystem/DDM/DDMFileSystem.cpp"

"DDM-Engine/Includes/STBIncludes.cpp")

target_include_directories(DDMEngine 
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_compile_definitions(DDMEngine PRIVATE DDMENGINE_BUILD)


# Include directories specific to this target
target_include_directories(DDMEngine  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_command(
    TARGET DDMEngine 
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_RUNTIME_DLLS:DDMEngine>
        $<TARGET_FILE_DIR:DDMEngine>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_PDB_FILE:DDMEngine>
        $<TARGET_FILE_DIR:DDMEngine>
    COMMAND_EXPAND_LISTS
)

if (MSVC)
  # Use CMake's built-in property to set warning level cleanly
  set_property(TARGET DDMEngine PROPERTY MSVC_WARNING_LEVEL 4)

  # Treat warnings as errors
  target_compile_options(DDMEngine PRIVATE /WX)

  # Suppress warning C26459 just for this target
  target_compile_options(DDMEngine PRIVATE /wd26459)
else()
  target_compile_options(DDMEngine PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
