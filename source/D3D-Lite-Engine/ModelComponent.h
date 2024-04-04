#ifndef ModelComponentIncluded
#define ModelComponentIncluded
#include "Component.h"
#include "Structs.h"


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

		std::vector<UniformBufferObject> m_Ubos{};

		std::vector<VkBuffer> m_UboBuffers{};
		std::vector<VkDeviceMemory> m_UbosMemory{};
		std::vector<void*> m_UbosMapped{};

		std::vector<VkDescriptorSet> m_DescriptorSets{};


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

		//Initialization functions
		void CreateVertexBuffer();
		void CreateIndexBuffer();
		void CreateUniformBuffers();

		void UpdateDescriptorSets();

		void UpdateUniformBuffer(uint32_t frame);

		//Texture functions
		D3D::PipelinePair& GetPipeline();

		void Cleanup();
	};
}
#endif // !ModelComponentIncluded