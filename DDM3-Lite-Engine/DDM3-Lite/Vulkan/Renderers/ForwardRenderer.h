// ForwardRenderer.h
// Renderer that will use the forward rendering technique

#ifndef _FORWARD_RENDERER_
#define _FORWARD_RENDERER_

// Parent include
#include "Vulkan/Renderers/Renderer.h"

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
	class ImGuiWrapper;
	class SyncObjectManager;

	class ForwardRenderer final : public Renderer
	{
	public:
		ForwardRenderer();

		~ForwardRenderer();

		// Delete copy and move operations
		ForwardRenderer(ForwardRenderer&) = delete;
		ForwardRenderer(ForwardRenderer&&) = delete;

		ForwardRenderer& operator=(ForwardRenderer&) = delete;
		ForwardRenderer& operator=(ForwardRenderer&& other) = delete;

		virtual void Render() override;
		  
		virtual VkExtent2D GetExtent() override;
		  
		virtual VkRenderPass GetRenderpass() override;

		virtual void AddDefaultPipelines();
	private:
		// Pointer to the renderpass wrapper
		std::unique_ptr<RenderpassWrapper> m_pRenderpassWrapper{};

		// Pointer to the swapchain wrapper
		std::unique_ptr<SwapchainWrapper> m_pSwapchainWrapper{};

		// Pointer to the sync object manager
		std::unique_ptr<SyncObjectManager> m_pSyncObjectManager{};

		// Pointer to the ImGui wrapper
		std::unique_ptr<ImGuiWrapper> m_pImGuiWrapper{};

		int m_AttachmentCount{ 1 };

		void InitImgui();

		void CleanupImgui();

		void RecordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex);

		void RecreateSwapChain();

		void CreateRenderpass(VkFormat swapchainFormat);
	};
}

#endif // !_FORWARD_RENDERER_
