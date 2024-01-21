#ifndef DescriptorPoolWrapperIncluded
#define DescriptorPoolWrapperIncluded

#include "GLFWIncludes.h"

#include <vector>

namespace D3D
{
	class ModelComponent;

	class DescriptorPoolWrapper
	{
	public:
		DescriptorPoolWrapper(uint32_t uboAmount, uint32_t textureAmount);
		~DescriptorPoolWrapper() = default;

		DescriptorPoolWrapper(DescriptorPoolWrapper& other) = delete;
		DescriptorPoolWrapper(DescriptorPoolWrapper&& other) = delete;

		DescriptorPoolWrapper& operator=(DescriptorPoolWrapper& other) = delete;
		DescriptorPoolWrapper& operator=(DescriptorPoolWrapper&& other) = delete;


		void Cleanup(VkDevice device);

		void AddModel(ModelComponent* pModel);
		void RemoveModel(ModelComponent* pModel);

		void CreateDescriptorSets(VkDescriptorSetLayout layout, std::vector<VkDescriptorSet>& descriptorSets);

		void UpdateDescriptorSets(std::vector<std::vector<VkBuffer>>& uboBuffers, std::vector<VkDeviceSize>& uboSizes, std::vector<VkDescriptorSet>& descriptorSets);

		void UpdateDescriptorSets(std::vector<std::vector<VkBuffer>>& uboBuffers, std::vector<VkDeviceSize>& uboSizes, std::vector<VkDescriptorSet>& descriptorSets, std::vector<VkImageView>& imageViews);

	private:
		size_t m_MaxFramesInFlight{};

		const uint32_t m_UboAmount;
		const uint32_t m_TextureAmount;

		int m_MaxDescriptorSets{ 8 };
		int m_IncreaseFactor{ 2 };

		int m_AllocatedDescriptorSets{};

		VkDescriptorPool m_DescriptorPool{};

		std::vector<ModelComponent*> m_pModels{};

		void InitDescriptorPool();
		void ResizeDescriptorPool();
	};
}

#endif // !DescriptorPoolWrapperIncluded