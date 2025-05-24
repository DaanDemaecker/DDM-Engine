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

		// Get the commandbuffer currently in use
		VkCommandBuffer& GetCurrentCommandBuffer();

		CommandpoolManager* GetCommandPoolManager();

		VkExtent2D GetExtent();

		VkRenderPass GetRenderpass();
	private:
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
	};
}

#endif // !_DEFAULT_RENDERER_
