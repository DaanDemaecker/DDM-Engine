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
	class BufferManager;
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

		// Create a buffer
		// Parameters:
		//     size: the size of the buffer
		//     usage: the usage flags for the buffer
		//     properties: the property flags for the buffer
		//     buffer: reference to the buffer to be made
		//     bufferMemory: reference to the memory of the buffer that will be made
		void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

		// Copy a buffer to another buffer
		// Parameters:
		//     srcBuffer: the source buffer
		//     dstBuffer: the destination buffer
		//     size: the size of the buffers
		void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

		// Create a vertex buffer
		// Parameters:
		//     vertices: reference to vector of vertices 
		//     vertexBuffer: handle to the vertex buffer to be created
		//     vertexBufferMemory: handle of the vertex buffer memory
		void CreateVertexBuffer(std::vector<DDM3::Vertex>& vertices, VkBuffer& vertexBuffer, VkDeviceMemory& vertexBufferMemory);

		// Create a vertex buffer
		// Parameters:
		//     indices: reference to vector of indices 
		//     indexBuffer: handle to the index buffer to be created
		//     indexBufferMemory: handle of the index buffer memory
		void CreateIndexBuffer(std::vector<uint32_t>& indices, VkBuffer& indexBuffer, VkDeviceMemory& indexBufferMemory);

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


		VkExtent2D GetExtent();

		VkRenderPass GetRenderpass();
	private:

		//--MultiSampling--
		VkSampleCountFlagBits m_MsaaSamples = VK_SAMPLE_COUNT_1_BIT;

		// Pointer to the buffer manager
		std::unique_ptr<BufferManager> m_pBufferManager{};

		// Pointer to the ImGui wrapper
		std::unique_ptr<ImGuiWrapper> m_pImGuiWrapper{};

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
