// DefferedRenderer.h
// Renderer that will use the deferred rendering technique

#ifndef _DEFERRED_RENDERER_
#define _DEFERRED_RENDERER_

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

	class DeferredRenderer final : public Renderer
	{
	public:
		DeferredRenderer();

		~DeferredRenderer();

		// Delete copy and move operations
		DeferredRenderer(DeferredRenderer&) = delete;
		DeferredRenderer(DeferredRenderer&&) = delete;

		DeferredRenderer& operator=(DeferredRenderer&) = delete;
		DeferredRenderer& operator=(DeferredRenderer&& other) = delete;

		virtual void Render() override;

		virtual VkExtent2D GetExtent() override;

		virtual VkRenderPass GetRenderpass() override;
	private:
		// Pointer to the renderpass wrapper
		std::unique_ptr<RenderpassWrapper> m_pRenderpassWrapper{};

		// Pointer to the swapchain wrapper
		std::unique_ptr<SwapchainWrapper> m_pSwapchainWrapper{};

		// Pointer to the sync object manager
		std::unique_ptr<SyncObjectManager> m_pSyncObjectManager{};

		// Pointer to the ImGui wrapper
		std::unique_ptr<ImGuiWrapper> m_pImGuiWrapper{};

		int m_AttachmentCount{ 3 };

		void InitImgui();

		void CleanupImgui();

		void RecordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex);

		void RecreateSwapChain();
	};
}

#endif // !_DEFERRED_RENDERER_
