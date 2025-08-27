// PipelineManager.cpp

// Header include
#include "PipelineManager.h"

// File includes
#include "Managers/ConfigManager.h"
#include "Vulkan/VulkanWrappers/PipelineWrapper.h"
#include "Vulkan/VulkanWrappers/DescriptorPoolWrapper.h"
#include "Vulkan/VulkanObject.h"

DDM::PipelineManager::PipelineManager()
{
	// Get config manager
	auto& configManager{ ConfigManager::GetInstance() };

	// Initialize default pipeline name 
	m_DefaultPipelineName = configManager.GetString("DefaultPipelineName");
}

DDM::PipelineManager::~PipelineManager()
{
}

void DDM::PipelineManager::AddDefaultPipeline(VkDevice device, VkRenderPass renderPass, VkSampleCountFlagBits sampleCount)
{
	

}

void DDM::PipelineManager::AddGraphicsPipeline(VkDevice device, VkRenderPass renderPass, VkSampleCountFlagBits sampleCount, const std::string& pipelineName, std::initializer_list<const std::string>& filePaths, bool hasDepthStencil, bool writesToDepth, int subpass)
{
	// Check if pipeline already exists, if it does, delete it
	if (m_GraphicPipelines.contains(pipelineName))
	{
		m_GraphicPipelines[pipelineName] = nullptr;
	}

	// Create a new pipeline in the correct spot in the map
	m_GraphicPipelines[pipelineName] = std::make_unique<DDM::PipelineWrapper>
		(device, renderPass, sampleCount, filePaths, hasDepthStencil, writesToDepth, subpass);
}

DDM::PipelineWrapper* DDM::PipelineManager::GetPipeline(const std::string& name)
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
