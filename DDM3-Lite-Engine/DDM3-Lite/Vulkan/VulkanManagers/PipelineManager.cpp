// PipelineManager.cpp

// Header include
#include "PipelineManager.h"

// File includes
#include "Managers/ConfigManager.h"
#include "Vulkan/VulkanWrappers/PipelineWrapper.h"
#include "Vulkan/VulkanWrappers/DescriptorPoolWrapper.h"
#include "Vulkan/VulkanObject.h"

DDM3::PipelineManager::PipelineManager()
{
	// Get config manager
	auto& configManager{ ConfigManager::GetInstance() };

	// Initialize default pipeline name 
	m_DefaultPipelineName = configManager.GetString("DefaultPipelineName");
}

DDM3::PipelineManager::~PipelineManager()
{
}

void DDM3::PipelineManager::AddDefaultPipeline(VkDevice device, VkRenderPass renderPass, VkSampleCountFlagBits sampleCount)
{
	

}

void DDM3::PipelineManager::AddGraphicsPipeline(VkDevice device, VkRenderPass renderPass, VkSampleCountFlagBits sampleCount, const std::string& pipelineName, std::initializer_list<const std::string>& filePaths, bool hasDepthStencil, bool writesToDepth, int subpass)
{
	// Check if pipeline already exists, if it does, delete it
	if (m_GraphicPipelines.contains(pipelineName))
	{
		m_GraphicPipelines[pipelineName] = nullptr;
	}

	// Create a new pipeline in the correct spot in the map
	m_GraphicPipelines[pipelineName] = std::make_unique<DDM3::PipelineWrapper>
		(device, renderPass, sampleCount, filePaths, hasDepthStencil, writesToDepth, subpass);
}

DDM3::PipelineWrapper* DDM3::PipelineManager::GetPipeline(const std::string& name)
{
	// Check if pipeline exists
	if (m_GraphicPipelines.contains(name))
	{
		// If it exists, return the correct pipeline
		return m_GraphicPipelines[name].get();
	}
	else
	{
		// If not, return default pipeline
		return m_GraphicPipelines[m_DefaultPipelineName].get();
	}
}
