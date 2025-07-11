// DeferredRenderer2.h
// Renderer that will use the deferred rendering technique

#ifndef _DEFERRED_RENDERER2_
#define _DEFERRED_RENDERER2_

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

	class DeferredRenderer2 final : public Renderer
	{
	public:
		DeferredRenderer2();

		~DeferredRenderer2();

		// Delete copy and move operations
		DeferredRenderer2(DeferredRenderer2&) = delete;
		DeferredRenderer2(DeferredRenderer2&&) = delete;

		DeferredRenderer2& operator=(DeferredRenderer2&) = delete;
		DeferredRenderer2& operator=(DeferredRenderer2&& other) = delete;

		virtual void Render() override;

		virtual VkExtent2D GetExtent() override;

		virtual RenderpassWrapper* GetDefaultRenderpass() override;

		virtual void AddDefaultPipelines();
	private:
		enum
		{
			kAttachment_BACK = 0,
			kAttachment_DEPTH = 1,
			kAtttachment_GBUFFER_ALBEDO = 2,
			kAtttachment_GBUFFER_NORMAL = 3,
			kAtttachment_GBUFFER_POSITION = 4
		};

		enum
		{
			kSubpass_DEPTH = 0,
			kSubpass_GBUFFER = 1,
			kSubpass_LIGHTING = 2
		};

		void CreateMasterRenderpass();

		VkRenderPass m_MasterRenderpass{};


		std::unique_ptr<RenderpassWrapper> m_pRenderpass{};

		void CreateRenderpass();

		void SetupAttachments();

		void SetupDepthPass();

		void SetupGeometryPass();

		void SetupLightingPass();

		void SetupDependencies();



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

		std::shared_ptr<Texture> m_pDepthTexture{};


		void InitImgui();

		void CleanupImgui();

		void RecordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex);

		void RecreateSwapChain();



		void CreateDescriptorSetLayout();

		void CreateDescriptorPool();
	};
}

#endif // !_DEFERRED_RENDERER2_
