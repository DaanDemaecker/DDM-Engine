// HBAORenderer.h
// This renderer will implement Horizon Based Ambient Occlusion

#ifndef _HBAO_RENDERER_
#define _HBAO_RENDERER_

// Parent includde
#include "Vulkan/Renderers/Renderer.h"

// File includes
#include "Vulkan/VulkanManagers/SyncObjectManager.h"
#include "DataTypes/Structs.h"
#include "DataTypes/DescriptorObjects/InputAttachmentDescriptorObject.h"

// Standard library includes
#include <memory>
#include <vector>

namespace DDM3
{
	// Class forward declarations
	class PipelineWrapper;
	class SwapchainWrapper;
	class RenderpassWrapper;
	class ImGuiWrapper;

	class HBAORenderer final : public Renderer
	{
	public:
		HBAORenderer();

		virtual ~HBAORenderer();

		// Delete copy and move operations
		HBAORenderer(HBAORenderer&) = delete;
		HBAORenderer(HBAORenderer&&) = delete;

		HBAORenderer& operator=(HBAORenderer&) = delete;
		HBAORenderer& operator=(HBAORenderer&& other) = delete;

		virtual void Render();

		virtual VkExtent2D GetExtent();

		virtual RenderpassWrapper* GetDefaultRenderpass();

		virtual void AddDefaultPipelines();
		enum
		{
			kSubpass_DEPTH = 0,
			kSubpass_GBUFFER = 1,
			kSubpass_AOGEN = 2,
			kSubpass_AOBLUR = 3,
			kSubpass_LIGHTING = 4,
			kSubpass_IMGUI = 5
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

		void SetupAOGenPass();

		void SetupAOBlurPass();

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

		void RecordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex);

		void RecreateSwapChain();

		void ResetDescriptorSets();



		void CreateDescriptorSetLayout();

		void CreateDescriptorPool();

		void CreateDescriptorSets();

	};
}

#endif // !_HBAO_RENDERER_
