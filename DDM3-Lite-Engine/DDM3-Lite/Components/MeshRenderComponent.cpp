#include "MeshRenderComponent.h"
#include "Vulkan/VulkanObject.h"
#include "../DataTypes/Materials/Material.h"
#include "TransformComponent.h"
#include "../Utils/Utils.h"
#include "../Vulkan/VulkanWrappers/DescriptorPoolWrapper.h"
#include "../DataTypes/Mesh.h"
#include "Vulkan/VulkanWrappers/PipelineWrapper.h"
#include "DDMModelLoader/Mesh.h"

DDM3::MeshRenderComponent::MeshRenderComponent()
{
	m_pMaterial = std::make_shared<DDM3::Material>();

	m_pUboDescriptorObject = std::make_unique<DDM3::UboDescriptorObject<UniformBufferObject>>();
}

void DDM3::MeshRenderComponent::EarlyUpdate()
{
	if (m_ShouldCreateDescriptorSets)
	{
		CreateDescriptorSets();
		m_ShouldCreateDescriptorSets = false;
	}

	if (m_pMaterial->ShouldUpdateDescriptorSets())
	{
		UpdateDescriptorSets();
	}

	UpdateUniformBuffer(VulkanObject::GetInstance().GetCurrentFrame());
}

void DDM3::MeshRenderComponent::SetMesh(std::shared_ptr<Mesh> pMesh)
{
	if (m_Initialized)
	{
		m_Initialized = false;
	}

	m_pMesh = pMesh;
	
	m_ShouldCreateDescriptorSets = true;

	CreateUniformBuffers();

	m_Initialized = true;

	m_IsTransparant = pMesh->IsTransparant();
}

void DDM3::MeshRenderComponent::SetMesh(DDMML::Mesh* pMesh)
{
	if (m_Initialized)
	{
		m_Initialized = false;
	}
	m_pMesh = std::make_shared<Mesh>(pMesh);
	m_ShouldCreateDescriptorSets = true;
	CreateUniformBuffers();
	m_Initialized = true;

	m_IsTransparant = pMesh->GetIsTransparant();
}

void DDM3::MeshRenderComponent::SetMaterial(std::shared_ptr<Material> pMaterial)
{
	m_pMaterial->GetDescriptorPool()->RemoveModel(this);
	m_pMaterial = pMaterial;
	
	m_ShouldCreateDescriptorSets = true;
}

void DDM3::MeshRenderComponent::RenderDepth()
{
	if (m_IsTransparant || m_pMesh == nullptr)
		return;

	auto frame{ VulkanObject::GetInstance().GetCurrentFrame() };

	m_pMesh->Render(VulkanObject::GetInstance().GetPipeline("Depth"), &m_DepthDescriptorSets[frame]);
}

void DDM3::MeshRenderComponent::Render()
{
	if (m_IsTransparant || m_pMesh == nullptr)
		return;

	auto frame{ VulkanObject::GetInstance().GetCurrentFrame() };

	m_pMesh->Render(GetPipeline(), &m_DescriptorSets[frame]);
}

void DDM3::MeshRenderComponent::RenderTransparancy()
{
	if (!m_IsTransparant || m_pMesh == nullptr)
		return;

	auto frame{ VulkanObject::GetInstance().GetCurrentFrame() };

	m_pMesh->Render(GetPipeline(), &m_DescriptorSets[frame]);
}

void DDM3::MeshRenderComponent::OnGUI()
{
	m_pMaterial->OnGUI();
}

void DDM3::MeshRenderComponent::CreateUniformBuffers()
{
	// Get reference to renderer
	auto& renderer = VulkanObject::GetInstance();
	// Get amount of frames
	auto frames = renderer.GetMaxFrames();

	// Resize ubos to amount of frames
	m_Ubos.resize(frames);
	// Resize dirty flages to amount of frames
	m_UboChanged.resize(frames);
}

void DDM3::MeshRenderComponent::CreateDescriptorSets()
{
	// Create descriptorsets
	m_pMaterial->CreateDescriptorSets(this, m_DescriptorSets);

	CreateDepthDescriptorSets();

	// Update descriptors
	UpdateDescriptorSets();
}

void DDM3::MeshRenderComponent::UpdateDescriptorSets()
{
	std::vector<DescriptorObject*> descriptors{ m_pUboDescriptorObject.get() };

	// Update descriptorsets
	m_pMaterial->UpdateDescriptorSets(m_DescriptorSets, descriptors);

	UpdateDepthDescriptorSets();
}

void DDM3::MeshRenderComponent::UpdateUniformBuffer(uint32_t frame)
{
	auto transform{ GetTransform() };

	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), transform->GetWorldPosition());

	glm::mat4 rotationMatrix = glm::mat4_cast(transform->GetWorldRotation());

	glm::mat4 scalingMatrix = glm::scale(glm::mat4(1.0f), transform->GetWorldScale());

	m_Ubos[frame].model = translationMatrix * rotationMatrix * scalingMatrix;

	VulkanObject::GetInstance().UpdateUniformBuffer(m_Ubos[frame]);


	m_pUboDescriptorObject->UpdateUboBuffer(m_Ubos[frame], frame);
}

DDM3::PipelineWrapper* DDM3::MeshRenderComponent::GetPipeline()
{
	if (m_pMaterial != nullptr)
	{
		return m_pMaterial->GetPipeline();
	}

	return VulkanObject::GetInstance().GetPipeline();
}

void DDM3::MeshRenderComponent::CreateDepthDescriptorSets()
{
	// Get pointer to the descriptorpool wrapper
	auto descriptorPool = VulkanObject::GetInstance().GetPipeline("Depth")->GetDescriptorPool();


	// Add model to descriptorpool wrapper
	descriptorPool->AddModel(this);
	// Create descriptorpools
	descriptorPool->CreateDescriptorSets(VulkanObject::GetInstance().GetPipeline("Depth")->GetDescriptorSetLayout(), m_DepthDescriptorSets);
}

void DDM3::MeshRenderComponent::UpdateDepthDescriptorSets()
{
	// Get pointer to the descriptorpool wrapper
	auto descriptorPool = VulkanObject::GetInstance().GetPipeline("Depth")->GetDescriptorPool();

	// Create list of descriptor objects and add the objects of the model to it
	std::vector<DescriptorObject*> descriptorObjectList{};


	std::vector<DescriptorObject*> descriptors{ m_pUboDescriptorObject.get() };


	// Update descriptorsets
	descriptorPool->UpdateDescriptorSets(m_DepthDescriptorSets, descriptors);
}
