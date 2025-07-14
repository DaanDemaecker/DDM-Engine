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
	class TextureDescriptorObject;

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

		virtual RenderpassWrapper* GetDefaultRenderpass() override;

		virtual void AddDefaultPipelines();
	private:
		enum
		{
			kDepthSubpass_DEPTH = 0
		};

		enum
		{
			kGeometrySubpass_ALBEDO = 0,
			kGeometrySubpass_NORMAL = 1,
			kGeometrySubpass_POSITION = 2,
			kGeometrySubpass_DEPTH = 3
		};

		enum
		{
			kLightingSubpass_COLOR = 0,
			kLightingSubpass_DEPTH = 1,
			kLightingSubpass_COLORRESOLVE = 2
		};


		std::unique_ptr<RenderpassWrapper> m_pDepthRenderpass{};

		// Pointer to the renderpass wrapper
		std::unique_ptr<RenderpassWrapper> m_pGeometryRenderpass{};

		// Pointer to the renderpass wrapper
		std::unique_ptr<RenderpassWrapper> m_pLightingRenderpass{};

		// Pointer to the swapchain wrapper
		std::unique_ptr<SwapchainWrapper> m_pSwapchainWrapper{};

		// Pointer to the sync object manager
		std::unique_ptr<SyncObjectManager> m_pSyncObjectManager{};

		// Pointer to the ImGui wrapper
		std::unique_ptr<ImGuiWrapper> m_pImGuiWrapper{};

		std::vector<std::unique_ptr<TextureDescriptorObject>> m_TextureDescriptorObjects{};

		// Descriptor sets
		VkPipelineLayout m_PipelineLayout{};

		std::vector<VkDescriptorSet> m_DescriptorSets{};

		VkDescriptorSetLayout m_DescriptorSetLayout{};

		VkDescriptorPool m_DescriptorPool{};

		std::vector<std::shared_ptr<Texture>> m_pDepthTextures{};


		void InitImgui();

		void CleanupImgui();

		void RecordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex);

		void RecreateSwapChain();



		void CreateDepthRenderpass();

		void CreateGeometryRenderpass();

		void CreateLightingRendepass(VkFormat swapchainFormat);



		void CreateDescriptorSetLayout();

		void CreateDescriptorPool();

		void CreateDescriptorSets();

		void SetupDescriptorSets();

		void UpdateDescriptorSets();

		void BindDescriptorSets(VkCommandBuffer commandBuffer);

		
		void TransitionImages(VkCommandBuffer commandBuffer, VkImageLayout oldLayout, VkImageLayout newLayout);
	};
}

#endif // !_DEFERRED_RENDERER_
