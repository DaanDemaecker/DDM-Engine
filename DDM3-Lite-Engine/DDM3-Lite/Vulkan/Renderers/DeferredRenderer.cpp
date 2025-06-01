// DeferredRenderer.cpp

// Header include 
#include "DeferredRenderer.h"

#include "Vulkan/VulkanUtils.h"
#include "Vulkan/VulkanObject.h"
#include "Vulkan/VulkanManagers/SyncObjectManager.h"
#include "Vulkan/VulkanWrappers/SwapchainWrapper.h"
#include "Vulkan/VulkanWrappers/RenderpassWrapper.h"
#include "Vulkan/VulkanWrappers/ImGuiWrapper.h"
#include "Vulkan/VulkanManagers/ImageManager.h"
#include "Vulkan/VulkanManagers/CommandpoolManager.h"
#include "Vulkan/VulkanManagers/PipelineManager.h"
#include "Vulkan/VulkanManagers/BufferCreator.h"
#include "Vulkan/VulkanWrappers/GPUObject.h"

#include "Engine/Window.h"

DDM3::DeferredRenderer::DeferredRenderer()
{
}

DDM3::DeferredRenderer::~DeferredRenderer()
{
}

void DDM3::DeferredRenderer::Render()
{
}

VkExtent2D DDM3::DeferredRenderer::GetExtent()
{
	return VkExtent2D();
}

VkRenderPass DDM3::DeferredRenderer::GetRenderpass()
{
	return VkRenderPass();
}

void DDM3::DeferredRenderer::InitImgui()
{
}

void DDM3::DeferredRenderer::CleanupImgui()
{
}

void DDM3::DeferredRenderer::RecordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex)
{
}

void DDM3::DeferredRenderer::RecreateSwapChain()
{
}
