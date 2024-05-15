#ifndef MeshRenderComponentIncluded
#define MeshRenderComponentIncluded

#include "../BaseClasses/Component.h"
#include "../DataTypes/Structs.h"
#include "../DataTypes/DescriptorObjects/UboDescriptorObject.h"


namespace D3D
{
	class Material;
	class Mesh;

	class MeshRenderComponent : public Component
	{
	public:
		MeshRenderComponent();
		virtual ~MeshRenderComponent() = default;

		virtual void EarlyUpdate() override;

		void SetMesh(std::shared_ptr<Mesh> pMesh);
		void SetMaterial(std::shared_ptr<Material> pMaterial);

		virtual void Render() override;

		virtual void OnGUI() override;

		void CreateDescriptorSets();


	protected:
		//Member variables
		bool m_Initialized{ false };

		// Vector for Uniform Buffer Objects
		std::vector<UniformBufferObject> m_Ubos{};
		// Vector of dirty flags for UBOs
		std::vector<bool> m_UboChanged{};

		std::unique_ptr<D3D::UboDescriptorObject<UniformBufferObject>> m_pUboDescriptorObject{};

		bool m_ShouldCreateDescriptorSets{ false };

		std::shared_ptr<Mesh> m_pMesh{};

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
	};
}
#endif // !MeshRenderComponentIncluded