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
#include "DataTypes/DescriptorObjects/UboDescriptorObject.h"


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
			kSubpass_AO_GEN = 2,
			kSubpass_LIGHTING = 3,
			kSubpass_IMGUI = 4
		};
	private:
		enum
		{
			kAttachment_BACK = 0,
			kAttachment_DEPTH = 1,
			kAttachment_GBUFFER_ALBEDO = 2,
			kAttachment_GBUFFER_NORMAL = 3,
			kAttachment_GBUFFER_POSITION = 4,
			kAttachment_AO_MAP = 5,
			kAttachment_AO_BLURRED = 6
		};


		std::unique_ptr<RenderpassWrapper> m_pRenderpass{};

		void CreateRenderpass();

		void SetupAttachments();

		void SetupDepthPass();

		void SetupGeometryPass();

		void SetupAoGenPass();

		void SetupLightingPass();

		void SetupImGuiPass();

		void SetupDependencies();


		// Pointer to the swapchain wrapper
		std::unique_ptr<SwapchainWrapper> m_pSwapchainWrapper{};

		// Pointer to the sync object manager
		std::unique_ptr<SyncObjectManager> m_pSyncObjectManager{};

		// Pointer to the ImGui wrapper
		std::unique_ptr<ImGuiWrapper> m_pImGuiWrapper{};


		
		PipelineWrapper* m_pLightingPipeline{};

		std::vector<std::unique_ptr<InputAttachmentDescriptorObject>> m_pLightingInputDescriptorObjects{};

		std::vector<VkDescriptorSet> m_LightingDescriptorSets{};

		VkDescriptorSetLayout m_LightingDescriptorSetLayout{};

		VkDescriptorPool m_LightingDescriptorPool{};


		PipelineWrapper* m_pAoPipeline{};

		std::vector<std::unique_ptr<InputAttachmentDescriptorObject>> m_pAoGenInputDescriptorObjects{};

		std::vector<VkDescriptorSet> m_AoGenDescriptorSets{};

		VkDescriptorSetLayout m_AoGenDescriptorSetLayout{};

		VkDescriptorPool m_AoGenDescriptorPool{};

		// Samples
		const int m_SampleCount{ 64 };
		std::vector<std::vector<glm::vec4>> m_Samples{};

		std::unique_ptr<UboDescriptorObject<glm::vec4>> m_pSamplesDescriptorObject{};

		void SetNewSamples(int frame);


		void InitImgui();

		void CleanupImgui();

		void RecordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex);

		void RecreateSwapChain();

		void ResetDescriptorSets();


		// Descriptor objects setup
		void SetupDescriptorObjects();

		void SetupDescriptorObjectsAoGen();

		void SetupDescriptorObjectsLighting();

		// Descriptor pools setup
		void CreateDescriptorPool();

		void CreateAoGenDescriptorPool();

		void CreateLightingDescriptorPool();

		// Descriptor Layouts setup
		void CreateDescriptorSetLayout();

		void CreateAoGenDescriptorSetLayout();

		void CreateLightingDescriptorSetLayout();


		// Descriptor sets setup
		void CreateDescriptorSets();

		void CreateAoGenDescriptorSets();

		void CreateLightingDescriptorSets();

		// Update Descriptor sets
		void UpdateDescriptorSets(int frame);

		void UpdateAoGenDescriptorSets(int frame);

		void UpdateLightingDescriptorSets(int frame);
	};
}

#endif // !_SSAO_RENDERER_
