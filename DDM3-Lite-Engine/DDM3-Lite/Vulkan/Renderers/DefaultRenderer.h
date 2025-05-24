// DefaultRenderer.h
// This class will be the default renderer for everything 

#ifndef _DEFAULT_RENDERER_
#define _DEFAULT_RENDERER_

// File includes
#include "Includes/VulkanIncludes.h"
#include "Vulkan/VulkanManagers/SyncObjectManager.h"
#include "DataTypes/Structs.h"

// Standard library includes
#include <memory>
#include <vector>

namespace DDM3
{
	// Class forward declarations
	class SwapchainWrapper;
	class RenderpassWrapper;
	class Viewport;
	class BufferCreator;
	class ImGuiWrapper;
	class PipelineManager;
	class PipelineWrapper;
	class ImageManager;
	class CommandpoolManager;
	class SyncObjectManager;

	class DefaultRenderer final
	{
	public:
		DefaultRenderer();

		~DefaultRenderer();

		// Delete copy and move operations
		DefaultRenderer(DefaultRenderer&) = delete;
		DefaultRenderer(DefaultRenderer&&) = delete;

		DefaultRenderer& operator=(DefaultRenderer&) = delete;
		DefaultRenderer& operator=(DefaultRenderer&& other) = delete;

		void Render();

		// Add a new graphics pipeline
		// Parameters:
		//     pipelineName: the name of the new pipeline
		//     filePaths: a list of shader file names for this pipeline
		//     hasDepthStencil: boolean that indicates if this pipeline needs a depth stencil, true by default
		void AddGraphicsPipeline(const std::string& pipelineName, std::initializer_list<const std::string>& filePaths,
			bool hasDepthStencil = true);

		//Get the default image view
		VkImageView& GetDefaultImageView();

		// Get the image sampler
		VkSampler& GetSampler();

		// Get the pipeline with the given name
		// Parameters:
		//     name: the name of the requested pipeline, "Default" by default
		PipelineWrapper* GetPipeline(const std::string & name = "Default");

		// Get the commandbuffer currently in use
		VkCommandBuffer& GetCurrentCommandBuffer();

		// Create a texture
		// Parameters:
		//     texture: reference to the texture object that will hold the texture
		//     textureName: textpath to the image
		void CreateTexture(Texture& texture, const std::string& textureName);

		// Create a cube texture
		// Parameters:
		//     cubeTexture: reference to the texture object
		//     textureNames: a list of the file paths for the cube faces in order: right,left,up,down,front,back
		void CreateCubeTexture(Texture& cubeTexture, const std::initializer_list<std::string const>& textureNames);

		CommandpoolManager* GetCommandPoolManager();

		VkExtent2D GetExtent();

		VkRenderPass GetRenderpass();
	private:

		//--MultiSampling--
		VkSampleCountFlagBits m_MsaaSamples = VK_SAMPLE_COUNT_1_BIT;

		// Pointer to thep pipeline manager
		std::unique_ptr<PipelineManager> m_pPipelineManager{};

		// Pointer to the image manager
		std::unique_ptr<ImageManager> m_pImageManager{};

		// Pointer to the commandpool manager
		std::unique_ptr<CommandpoolManager> m_pCommandPoolManager{};

		// Pointer to the renderpass wrapper
		std::unique_ptr<RenderpassWrapper> m_pRenderpassWrapper{};

		// Pointer to the swapchain wrapper
		std::unique_ptr<SwapchainWrapper> m_pSwapchainWrapper{};

		// Pointer to the sync object manager
		std::unique_ptr<SyncObjectManager> m_pSyncObjectManager{};

		// Pointer to the ImGui wrapper
		std::unique_ptr<ImGuiWrapper> m_pImGuiWrapper{};

		void InitImgui();

		void CleanupImgui();

		void RecordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex);

		void RecreateSwapChain();

		// Begin a command buffer for a single command
		VkCommandBuffer BeginSingleTimeCommands();

		// End a command buffer after a single command
		// Parameters:
		//     commandBuffer: handle of the buffer to be ended
		void EndSingleTimeCommands(VkCommandBuffer commandBuffer);

		// Transition an image frome one layout to another
	   // Parameters:
	   //     image: the given image
	   //     format: the format of the image
	   //     oldLayout: the old layout of the image
	   //     newLayout: the new layout of the image
	   //     mipLevels: the amount of mipmaps in this image
		void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);

		void AddDefaultPipeline();

	};
}

#endif // !_DEFAULT_RENDERER_
