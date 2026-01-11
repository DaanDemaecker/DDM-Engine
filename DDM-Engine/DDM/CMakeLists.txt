# Create the executable
add_executable(
	${EXECUTABLE_NAME}

"BaseClasses/GameObject.cpp"

"CustomComponents/SpectatorMovement.cpp"
"CustomComponents/Rotator.cpp"
"CustomComponents/ModelLoader.cpp"
"CustomComponents/MaterialSwitcher/MaterialSwitcher.cpp"
"CustomComponents/MaterialSwitcher/MaterialSwitchManager.cpp"
"CustomComponents/InfoComponent.cpp"
"CustomComponents/Audio/AudioTester.cpp"

"EngineComponents/Camera.cpp"
"EngineComponents/Light/LightComponent.cpp"
"EngineComponents/MeshRenderer.cpp"
"EngineComponents/SkyBox.cpp"
"EngineComponents/Transform.cpp"
"EngineComponents/Audio/AudioSource.cpp"
"EngineComponents/Audio/AudioClip.cpp"

"Events/Subject.cpp"

"DataTypes/DescriptorObjects/InputAttachmentDescriptorObject.cpp"
"DataTypes/DescriptorObjects/TextureDescriptorObject.cpp"

"DataTypes/Materials/Material.cpp"
"DataTypes/Materials/CubeMapMaterial.cpp"
"DataTypes/Materials/MultiMaterial.cpp"
"DataTypes/Materials/TexturedMaterial.cpp"

"Engine/DDMEngine.cpp"
"Engine/main.cpp"
"Engine/Scene.cpp"
"Engine/Window.cpp"
"Engine/DDMModelLoader.cpp"

"Managers/ConfigManager.cpp"
"Managers/SceneManager.cpp"
"Managers/TimeManager.cpp"
"Managers/ResourceManager.cpp"
"Managers/InputManager.cpp"
"Managers/Input/KeyboardImpl.cpp"
"Managers/Input/MouseImpl.h"
"Managers/Input/MouseImpl.cpp"
"Managers/Input/Mouse.cpp"

"Utils/Utils.cpp"

"Vulkan/VulkanObject.cpp"
"Vulkan/VulkanUtils.cpp"

"Vulkan/SpirVReflect/spirv_reflect.cpp"

"Vulkan/Renderers/ForwardRenderer.cpp"
"Vulkan/Renderers/DeferredRenderer.cpp"
"Vulkan/Renderers/AORenderers/SSAORenderer.cpp"
"Vulkan/Renderers/AORenderers/HBAORenderer.cpp"
"Vulkan/Renderers/AORenderers/GTAORenderer.cpp"

"Vulkan/VulkanManagers/BufferCreator.cpp"
"Vulkan/VulkanManagers/CommandpoolManager.cpp"
"Vulkan/VulkanManagers/ImageManager/ImageManager.cpp"
"Vulkan/VulkanManagers/ImageViewManager.cpp"
"Vulkan/VulkanManagers/PipelineManager.cpp"
"Vulkan/VulkanManagers/SyncObjectManager.cpp"
"Vulkan/VulkanManagers/ImageManager/STBImage.cpp"

"Vulkan/VulkanWrappers/DescriptorPoolWrapper.cpp"
"Vulkan/VulkanWrappers/GPUObject.cpp"
"Vulkan/VulkanWrappers/ImGuiWrapper.cpp"
"Vulkan/VulkanWrappers/InstanceWrapper.cpp"
"Vulkan/VulkanWrappers/PipelineWrapper"
"Vulkan/VulkanWrappers/RenderpassWrapper.cpp"
"Vulkan/VulkanWrappers/ShaderModuleWrapper.cpp"
"Vulkan/VulkanWrappers/SurfaceWrapper.cpp" 
"Vulkan/VulkanWrappers/SwapchainWrapper.cpp"
"Vulkan/VulkanWrappers/VulkanCore.cpp"
"Vulkan/VulkanWrappers/Mesh.cpp"
"Vulkan/VulkanWrappers/QueryPool.cpp"
"Vulkan/VulkanWrappers/Attachment.cpp"
"Vulkan/VulkanWrappers/FrameBuffer.cpp"
"Vulkan/VulkanWrappers/Subpass.cpp"
"Vulkan/VulkanWrappers/Image.cpp"
"Vulkan/VulkanWrappers/Buffer.cpp"

"ServiceLocator/ServiceLocator.cpp"

"ServiceLocator/SoundSystem/Fmod/FmodSoundSystem.cpp"
"ServiceLocator/SoundSystem/Fmod/FmodSystem.cpp"
"ServiceLocator/SoundSystem/Fmod/FmodChannel.cpp"

"Includes/STBIncludes.cpp")


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