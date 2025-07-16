// SSAORenderer.h
// Renderer that will use the deferred rendering technique

#ifndef _SSAO_RENDERER_
#define _SSAO_RENDERER_

// Parent include
#include "Vulkan/Renderers/Renderer.h"

// File includes
#include "Includes/VulkanIncludes.h"
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
	class SyncObjectManager;

	class SSAORenderer final : public Renderer
	{
	public:
		SSAORenderer();

		~SSAORenderer();

		// Delete copy and move operations
		SSAORenderer(SSAORenderer&) = delete;
		SSAORenderer(SSAORenderer&&) = delete;

		SSAORenderer& operator=(SSAORenderer&) = delete;
		SSAORenderer& operator=(SSAORenderer&&) = delete;

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

#endif // !_SSAO_RENDERER_
