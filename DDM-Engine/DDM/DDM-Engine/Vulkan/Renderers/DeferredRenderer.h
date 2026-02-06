// DeferredRenderer.h
// Renderer that will use the deferred rendering technique

#ifndef _DEFERRED_RENDERER_
#define _DEFERRED_RENDERER_

// Parent include
#include "DDM-Engine/Vulkan/Renderers/Renderer.h"

// File includes
#include "DDM-Engine/Vulkan/VulkanManagers/SyncObjectManager.h"
#include "DDM-Engine/DataTypes/Structs.h"
#include "DDM-Engine/DataTypes/DescriptorObjects/InputAttachmentDescriptorObject.h"

// Export include
#include "DDM-Engine/Export.h"

// Standard library includes
#include <memory>
#include <vector>

namespace DDM
{
	// Class forward declarations
	class PipelineWrapper;
	class SwapchainWrapper;
	class RenderpassWrapper;
	class ImGuiWrapper;

	class DDM_API DeferredRenderer final : public Renderer
	{
	public:
		DeferredRenderer();

		~DeferredRenderer();

		// Delete copy and move operations
		DeferredRenderer(DeferredRenderer&) = delete;
		DeferredRenderer(DeferredRenderer&&) = delete;

		DeferredRenderer& operator=(DeferredRenderer&) = delete;
		DeferredRenderer& operator=(DeferredRenderer&& other) = delete;

		virtual void Setup() override;

		virtual void Render() override;

		virtual VkExtent2D GetExtent() override;

		virtual RenderpassWrapper* GetDefaultRenderpass() override;

		virtual void AddDefaultPipelines();

		enum
		{
			kSubpass_DEPTH = 0,
			kSubpass_GBUFFER = 1,
			kSubpass_LIGHTING = 2,
			kSubpass_IMGUI = 3
		};
	private:
		enum
		{
			kAttachment_BACK = 0,
			kAttachment_DEPTH = 1,
			kAttachment_GBUFFER_ALBEDO = 2,
			kAttachment_GBUFFER_NORMAL = 3,
			kAttachment_GBUFFER_POSITION = 4
		};


		std::unique_ptr<RenderpassWrapper> m_pRenderpass{};

		void CreateRenderpass();

		void SetupAttachments();

		void SetupDepthPass();

		void SetupGeometryPass();

		void SetupLightingPass();

		void SetupImGuiPass();

		void SetupDependencies();

		void SetupDescriptorObjects();



		// Pointer to the swapchain wrapper
		std::unique_ptr<SwapchainWrapper> m_pSwapchainWrapper{};

		// Pointer to the sync object manager
		std::unique_ptr<SyncObjectManager> m_pSyncObjectManager{};

		// Pointer to the ImGui wrapper
		std::unique_ptr<ImGuiWrapper> m_pImGuiWrapper{};

		std::vector<std::unique_ptr<InputAttachmentDescriptorObject>> m_pInputAttachmentList{};


		
		PipelineWrapper* m_pLightingPipeline{};

		std::vector<VkDescriptorSet> m_DescriptorSets{};

		VkDescriptorSetLayout m_DescriptorSetLayout{};

		VkDescriptorPool m_DescriptorPool{};



		void InitImgui();

		void CleanupImgui();

		void RecordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex);

		void RecreateSwapChain();

		void ResetDescriptorSets();



		void CreateDescriptorSetLayout();

		void CreateDescriptorPool();

		void CreateDescriptorSets();
	};
}

#endif // !_DEFERRED_RENDERER_
