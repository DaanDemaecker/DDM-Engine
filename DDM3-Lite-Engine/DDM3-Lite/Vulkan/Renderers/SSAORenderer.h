// SSAORenderer.h
// This class will serve as a renderer that uses Screen Space Ambient Occlusion

#ifndef _SSAO_RENDERER_
#define _SSAO_RENDERER_

// Parent include
#include "Vulkan/Renderers/Renderer.h"

namespace DDM3
{
	class RenderpassWrapper;
	class SwapchainWrapper;
	class SyncObjectManager;
	class ImGuiWrapper;

	class SSAORenderer final : public Renderer
	{
	public:
		SSAORenderer();

		virtual ~SSAORenderer();

		virtual void Render() override;

		virtual VkRenderPass GetRenderpass() override;

		virtual VkExtent2D GetExtent() override;

	private:

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

	};
}

#endif // !_SSAO_RENDERER_

