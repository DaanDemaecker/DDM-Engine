#ifndef ModelComponentIncluded
#define ModelComponentIncluded
#include "Component.h"
#include "Structs.h"
#include "UboDescriptorObject.h"


namespace D3D
{
	class Material;

	class ModelComponent final : public Component
	{
	public:
		ModelComponent();
		~ModelComponent();

		void LoadModel(const std::string& textPath);
		void SetMaterial(std::shared_ptr<Material> pMaterial);

		virtual void Render() override;

		//Getters for rendering
		const VkBuffer& GetVertexBuffer() const { return m_VertexBuffer; }
		const VkBuffer& GetIndexBuffer() const { return m_IndexBuffer; }
		size_t GetIndexAmount() const { return m_Indices.size(); }

		void CreateDescriptorSets();


	private:
		//Member variables
		bool m_Initialized{ false };

		// Vector for Uniform Buffer Objects
		std::vector<UniformBufferObject> m_Ubos{};
		// Vector of dirty flags for UBOs
		std::vector<bool> m_UboChanged{};

		std::unique_ptr<D3D::UboDescriptorObject<UniformBufferObject>> m_pUboDescriptorObject{};

		bool m_ShouldCreateUniformBuffers{ false };

		std::vector<Vertex> m_Vertices{};
		std::vector<uint32_t> m_Indices{};

		//Vertex buffer
		VkBuffer m_VertexBuffer{};
		VkDeviceMemory m_VertexBufferMemory{};

		//Index buffer
		VkBuffer m_IndexBuffer{};
		VkDeviceMemory m_IndexBufferMemory{};

		//Material
		std::shared_ptr<Material> m_pMaterial{};


		// Vector of descriptorsets
		std::vector<VkDescriptorSet> m_DescriptorSets{};

		//Initialization functions
		void CreateUniformBuffers();

		void UpdateDescriptorSets();

		void UpdateUniformBuffer(uint32_t frame);

		//Texture functions
		D3D::PipelineWrapper* GetPipeline();

		void Cleanup();
	};
}
#endif // !ModelComponentIncluded