# Create the executable
add_executable(${EXECUTABLE_NAME}

"BaseClasses/GameObject.cpp"
"Components/CameraComponent.cpp"
"Components/DirectionalLightComponent.cpp"
"Components/MeshRenderComponent.cpp"
"Components/RotatorComponent.cpp"
"Components/SkyBoxComponent.cpp"
"Components/SpectatorMovementComponent.cpp"
"Components/TransformComponent.cpp"
"DataTypes/DescriptorObjects/TextureDescriptorObject.cpp"
"DataTypes/Materials/CubeMapMaterial.cpp"
"DataTypes/Materials/Material.cpp"
"DataTypes/Materials/TexturedMaterial.cpp"
"DataTypes/Mesh.cpp"
"Engine/DDM3Engine.cpp"
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
"Components/ModelLoaderComponent.cpp"
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
 "DataTypes/DescriptorObjects/InputAttachmentDescriptorObject.cpp")


# Include directories specific to this target
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})


# Suppress warning C26459 for this specific target
if(MSVC)
    target_compile_options(${EXECUTABLE_NAME} PRIVATE /wd26459)
endif()