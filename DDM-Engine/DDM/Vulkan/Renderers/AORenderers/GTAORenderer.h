// SSAORenderer.h
// Renderer that will use the deferred rendering technique with Horizon Based Ambient Occlusion (HBAO)

#ifndef _GTAO_RENDERER_
#define _GTAO_RENDERER_

// Parent include
#include "Vulkan/Renderers/Renderer.h"

// File includes
#include "Includes/VulkanIncludes.h"
#include "Vulkan/VulkanManagers/SyncObjectManager.h"
#include "DataTypes/Structs.h"
#include "DataTypes/DescriptorObjects/InputAttachmentDescriptorObject.h"
#include "DataTypes/DescriptorObjects/UboDescriptorObject.h"
#include "DataTypes/DescriptorObjects/TextureDescriptorObject.h"


namespace DDM
{
	// Class forward declarations
	class PipelineWrapper;
	class SwapchainWrapper;
	class ImGuiWrapper;
	class SyncObjectManager;

	class GTAORenderer final : public Renderer
	{
	public:
		GTAORenderer();

		~GTAORenderer();

		// Delete copy and move operations
		GTAORenderer(GTAORenderer&) = delete;
		GTAORenderer(GTAORenderer&&) = delete;

		GTAORenderer& operator=(GTAORenderer&) = delete;
		GTAORenderer& operator=(GTAORenderer&&) = delete;

		virtual void Render() override;

		virtual VkExtent2D GetExtent() override;

		virtual RenderpassWrapper* GetDefaultRenderpass() override;

		virtual void AddDefaultPipelines();

	private:
		std::unique_ptr<RenderpassWrapper> m_pRenderpass{};

		void CreateRenderpass();

		void SetupAttachments();

		void SetupDepthPass();

		void SetupGBufferPass();

		void SetupAoGenPass();

		void SetupAoBlurPass();

		void SetupLightingPass();

		void SetupImGuiPass();

		void SetupDependencies();


		// Pointer to the swapchain wrapper
		std::unique_ptr<SwapchainWrapper> m_pSwapchainWrapper{};

		// Pointer to the sync object manager
		std::unique_ptr<SyncObjectManager> m_pSyncObjectManager{};

		// Pointer to the ImGui wrapper
		std::unique_ptr<ImGuiWrapper> m_pImGuiWrapper{};


		// Everything needed for the AO descriptor sets
		PipelineWrapper* m_pAoPipeline{};

		std::vector<std::unique_ptr<InputAttachmentDescriptorObject>> m_pAoGenInputDescriptorObjects{};

		std::vector<VkDescriptorSet> m_AoGenDescriptorSets{};

		VkDescriptorSetLayout m_AoGenDescriptorSetLayout{};

		VkDescriptorPool m_AoGenDescriptorPool{};

		// Position texture
		std::unique_ptr<TextureDescriptorObject> m_pPositionTextureDescriptorObject{};


		// Everything needed for AOBlur Descriptorsets
		PipelineWrapper* m_pAoBlurPipeline{};

		std::vector<VkDescriptorSet> m_AoBlurDescriptorSets{};

		VkDescriptorSetLayout m_AoBlurDescriptorSetLayout{};

		VkDescriptorPool m_AoBlurDescriptorPool{};

		// AOGen texture
		std::unique_ptr<TextureDescriptorObject> m_pAoGenTextureDescriptorObject{};

		// Projection matrix
		std::unique_ptr<UboDescriptorObject<glm::mat4>> m_pViewMatrixDescObject{};


		// Everything needed for the Lighting descriptor sets
		PipelineWrapper* m_pLightingPipeline{};

		std::vector<std::unique_ptr<InputAttachmentDescriptorObject>> m_pLightingInputDescriptorObjects{};

		std::vector<VkDescriptorSet> m_LightingDescriptorSets{};

		VkDescriptorSetLayout m_LightingDescriptorSetLayout{};

		VkDescriptorPool m_LightingDescriptorPool{};

		void SetupPositionTexture();


		void InitImgui();

		void CleanupImgui();



		void RecordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex);

		void RecreateSwapChain();

		void ResetDescriptorSets();


		// Descriptor objects setup
		void SetupDescriptorObjects();

		void SetupDescriptorObjectsAoGen();

		void SetupDescriptorObjectsAoBlur();

		void SetupDescriptorObjectsLighting();

		// Descriptor pools setup
		void CreateDescriptorPools();

		void CreateAoGenDescriptorPool();

		void CreateAoBlurDescriptorPool();

		void CreateLightingDescriptorPool();

		// Descriptor Layouts setup
		void CreateDescriptorSetLayouts();

		void CreateAoGenDescriptorSetLayout();

		void CreateAoBlurDescriptorSetLayout();

		void CreateLightingDescriptorSetLayout();


		// Descriptor sets setup
		void CreateDescriptorSets();

		void CreateAoGenDescriptorSets();

		void CreateAoBlurDescriptorSets();

		void CreateLightingDescriptorSets();

		// Update Descriptor sets
		void UpdateDescriptorSets(int frame, int swapchainIndex);

		void UpdateAoGenDescriptorSets(int frame, int swapchainIndex);

		void UpdateAoBlurDescriptorSets(int frame, int swapchainIndex);

		void UpdateLightingDescriptorSets(int frame);
	};
}

#endif // !_HBAO_RENDERER_
