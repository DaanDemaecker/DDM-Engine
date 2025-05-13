#ifndef MeshRenderComponentIncluded
#define MeshRenderComponentIncluded

#include "../BaseClasses/Component.h"
#include "../DataTypes/Structs.h"
#include "../DataTypes/DescriptorObjects/UboDescriptorObject.h"

#include <memory>
#include <vector>

namespace DDMML
{
	class Mesh;
}

namespace DDM3
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
		void SetMesh(DDMML::Mesh* pMesh);
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

		std::unique_ptr<DDM3::UboDescriptorObject<UniformBufferObject>> m_pUboDescriptorObject{};

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
		DDM3::PipelineWrapper* GetPipeline();
	};
}
#endif // !MeshRenderComponentIncluded