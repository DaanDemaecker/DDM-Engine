# Create the executable
add_executable(${EXECUTABLE_NAME}

"BaseClasses/GameObject.cpp"
"Components/Camera.cpp"
"Components/Light/LightComponent.cpp"
"Components/MeshRenderer.cpp"
"Components/Rotator.cpp"
"Components/SkyBoxComponent.cpp"
"Components/SpectatorMovementComponent.cpp"
"Components/TransformComponent.cpp"
"DataTypes/DescriptorObjects/TextureDescriptorObject.cpp"
"DataTypes/Materials/CubeMapMaterial.cpp"
"DataTypes/Materials/Material.cpp"
"DataTypes/Materials/TexturedMaterial.cpp"
"DataTypes/Mesh.cpp"
"Engine/DDMEngine.cpp"
"Engine/main.cpp"
"Engine/Scene.cpp"
"Engine/Window.cpp"

"Managers/ConfigManager.cpp"
"Managers/SceneManager.cpp"
"Managers/TimeManager.cpp"

"Utils/Utils.cpp"

"Vulkan/SpirVReflect/spirv_reflect.cpp"
"Vulkan/VulkanManagers/BufferCreator.cpp"
"Vulkan/VulkanManagers/CommandpoolManager.cpp"
"Vulkan/VulkanManagers/ImageManager.cpp"
"Vulkan/VulkanManagers/ImageViewManager.cpp"
"Vulkan/VulkanManagers/PipelineManager.cpp"
"Vulkan/VulkanManagers/SyncObjectManager.cpp"
"Vulkan/VulkanWrappers/DescriptorPoolWrapper.cpp"
"Vulkan/VulkanWrappers/GPUObject.cpp"
"Vulkan/VulkanWrappers/ImGuiWrapper.cpp"
"Vulkan/VulkanWrappers/InstanceWrapper.cpp"
"Vulkan/VulkanWrappers/PipelineWrapper"
"Vulkan/VulkanWrappers/RenderpassWrapper.cpp"
"Vulkan/VulkanWrappers/ShaderModuleWrapper.cpp"
"Vulkan/VulkanWrappers/SurfaceWrapper.cpp" 
"Vulkan/VulkanWrappers/SwapchainWrapper.cpp"
"Vulkan/VulkanObject.cpp"
"Vulkan/VulkanUtils.cpp"
"Vulkan/Renderers/ForwardRenderer.cpp"
"DataTypes/Materials/MultiMaterial.cpp"
"Components/ModelLoader.cpp"
"Managers/ResourceManager.cpp"
"Managers/InputManager.cpp"
"Managers/Input/KeyboardImpl.cpp"
"Managers/Input/MouseImpl.h"
"Managers/Input/MouseImpl.cpp"
"Engine/DDMModelLoader.cpp"
"Components/MaterialSwitcher/MaterialSwitcher.cpp"
"Components/MaterialSwitcher/MaterialSwitchManager.cpp"
"Components/InfoComponent.cpp"
 "Vulkan/VulkanWrappers/VulkanCore.cpp"
 
 "Vulkan/VulkanWrappers/Attachment.cpp"
 "Vulkan/VulkanWrappers/FrameBuffer.cpp"
 "Vulkan/Renderers/DeferredRenderer.cpp"
 "Vulkan/VulkanWrappers/Subpass.cpp"
 "DataTypes/DescriptorObjects/InputAttachmentDescriptorObject.cpp"
 "Vulkan/Renderers/AORenderers/SSAORenderer.cpp"
 "Vulkan/Renderers/AORenderers/HBAORenderer.cpp"
 "Vulkan/Renderers/AORenderers/GTAORenderer.cpp"
 "Vulkan/VulkanWrappers/QueryPool.cpp" "Components/Light/Light.h")


# Include directories specific to this target
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if (MSVC)
  # Use CMake's built-in property to set warning level cleanly
  set_property(TARGET ${EXECUTABLE_NAME} PROPERTY MSVC_WARNING_LEVEL 4)

  # Treat warnings as errors
  target_compile_options(${EXECUTABLE_NAME} PRIVATE /WX)

  # Suppress warning C26459 just for this target
  target_compile_options(${EXECUTABLE_NAME} PRIVATE /wd26459)
else()
  target_compile_options(${EXECUTABLE_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()