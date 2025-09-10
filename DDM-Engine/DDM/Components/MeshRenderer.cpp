// MeshRenderer.cpp

// Header include
#include "MeshRenderer.h"

// File includes
#include "Vulkan/VulkanObject.h"
#include "Vulkan/VulkanWrappers/DescriptorPoolWrapper.h"
#include "Vulkan/VulkanWrappers/PipelineWrapper.h"
#include "Vulkan/VulkanWrappers/Mesh.h"

#include "DataTypes/Materials/Material.h"

#include "Utils/Utils.h"

#include "Components/Transform.h"

#include "DDMModelLoader/Mesh.h"

#include "Managers/ResourceManager.h"

DDM::MeshRenderComponent::MeshRenderComponent()
{
	// Create ubo descriptor object
	m_pUboDescriptorObject = std::make_unique<DDM::UboDescriptorObject<UniformBufferObject>>();

	// Get default material
	m_pMaterial = DDM::ResourceManager::GetInstance().GetDefaultMaterial();
}

void DDM::MeshRenderComponent::EarlyUpdate()
{
	// If the descriptorsets should be created, create them
	if (m_ShouldCreateDescriptorSets)
	{
		CreateDescriptorSets();
		m_ShouldCreateDescriptorSets = false;
	}

	// If the material indicates that the descriptorsets should be updated, update them
	if (m_pMaterial->ShouldUpdateDescriptorSets())
	{
		UpdateDescriptorSets();
	}

	// Update the uniform buffer of the current frame
	UpdateUniformBuffer(VulkanObject::GetInstance().GetCurrentFrame());
}

void DDM::MeshRenderComponent::SetMesh(std::shared_ptr<Mesh> pMesh)
{
	// If already initialized, set to false
	if (m_Initialized)
	{
		m_Initialized = false;
	}

	// Set new mesh and check if it is transparant
	m_pMesh = pMesh;
	m_IsTransparant = pMesh->IsTransparant();
	
	// Indicate that the descriptorsets should be created
	m_ShouldCreateDescriptorSets = true;

	// Create uniform buffers
	CreateUniformBuffers();

	// Indicate that component is initialized
	m_Initialized = true;
}

void DDM::MeshRenderComponent::SetMesh(DDMML::Mesh* pMesh)
{
	// Convert DDMML mesh to DDM mesh and propogate to overloaded function
	auto pNewMesh = std::make_shared<Mesh>(pMesh);
	SetMesh(pNewMesh);
}

void DDM::MeshRenderComponent::SetMaterial(std::shared_ptr<Material> pMaterial)
{
	// Remove model from current descriptorpool
	m_pMaterial->GetDescriptorPool()->RemoveModel(this);

	// Set new material
	m_pMaterial = pMaterial;
	
	// Indicate that the descriptorsets should be created
	m_ShouldCreateDescriptorSets = true;
}

void DDM::MeshRenderComponent::RenderDepth()
{
	// If transparant, don't render in depth pass
	// If no mesh, don't render
	if (m_IsTransparant || m_pMesh == nullptr)
		return;

	// Get index of current frame in flight
	auto frame{ VulkanObject::GetInstance().GetCurrentFrame() };

	// Render the mesh with the depth pipeline and the depth descriptorsets
	m_pMesh->Render(GetDepthPipeline(), &m_DepthDescriptorSets[frame]);
}

void DDM::MeshRenderComponent::Render()
{
	// If transparant, don't render in normal pass
	// If no mesh, don't render
	if (m_IsTransparant || m_pMesh == nullptr)
		return;

	// Get index of current frame in flight
	auto frame{ VulkanObject::GetInstance().GetCurrentFrame() };

	// Render with given pipeline and descriptorsets
	m_pMesh->Render(GetPipeline(), &m_DescriptorSets[frame]);
}

void DDM::MeshRenderComponent::RenderTransparancy()
{
	// If not transparant, don't render in transparancy pass
	// If no mesh, don't render
	if (!m_IsTransparant || m_pMesh == nullptr)
		return;

	// Get index of current frame in flight
	auto frame{ VulkanObject::GetInstance().GetCurrentFrame() };

	// Render with given pipeline and descriptorsets
	m_pMesh->Render(GetPipeline(), &m_DescriptorSets[frame]);
}

void DDM::MeshRenderComponent::OnGUI()
{
	// Propogate to material
	m_pMaterial->OnGUI();
}

void DDM::MeshRenderComponent::CreateUniformBuffers()
{
	// Get max amount of frames in flight
	auto frames = VulkanObject::GetInstance().GetMaxFrames();

	// Resize ubos to amount of frames
	m_Ubos.resize(frames);
	// Resize dirty flages to amount of frames
	m_UboChanged.resize(frames);
}

void DDM::MeshRenderComponent::CreateDescriptorSets()
{
	// Create descriptorsets
	m_pMaterial->CreateDescriptorSets(this, m_DescriptorSets);

	CreateDepthDescriptorSets();

	// Update descriptors
	UpdateDescriptorSets();
}

void DDM::MeshRenderComponent::UpdateDescriptorSets()
{
	// Create list of descriptor objects and add the objects of the model to it
	std::vector<DescriptorObject*> descriptors{ m_pUboDescriptorObject.get() };

	// Update descriptorsets
	m_pMaterial->UpdateDescriptorSets(m_DescriptorSets, descriptors);

	// Update depth descriptorsets
	UpdateDepthDescriptorSets();
}

void DDM::MeshRenderComponent::UpdateUniformBuffer(uint32_t frame)
{
	// Get transform
	auto transform{ GetTransform() };

	// Set up translation matrix
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), transform->GetWorldPosition());

	// Set up rotation matrix
	glm::mat4 rotationMatrix = glm::mat4_cast(transform->GetWorldRotation());

	// Set up scaling matrix
	glm::mat4 scalingMatrix = glm::scale(glm::mat4(1.0f), transform->GetWorldScale());

	// Calculate model matrix
	m_Ubos[frame].model = translationMatrix * rotationMatrix * scalingMatrix;

	// Update the uniform buffer object in the vulkan object
	VulkanObject::GetInstance().UpdateUniformBuffer(m_Ubos[frame]);

	// Update the ubo descriptor object
	m_pUboDescriptorObject->UpdateUboBuffer(&m_Ubos[frame], frame);
}

DDM::PipelineWrapper* DDM::MeshRenderComponent::GetPipeline()
{
	// If material is set, get pipeline from material, else get default pipeline
	if (m_pMaterial != nullptr)
	{
		return m_pMaterial->GetPipeline();
	}

	return VulkanObject::GetInstance().GetPipeline();
}

void DDM::MeshRenderComponent::CreateDepthDescriptorSets()
{
	// Get pointer to the descriptorpool wrapper
	auto descriptorPool =  GetDepthPipeline()->GetDescriptorPool();

	// Add model to descriptorpool wrapper
	descriptorPool->AddModel(this);
	// Create descriptorpools
	descriptorPool->CreateDescriptorSets(GetDepthPipeline()->GetDescriptorSetLayout(), m_DepthDescriptorSets);
}

void DDM::MeshRenderComponent::UpdateDepthDescriptorSets()
{
	// Get pointer to the descriptorpool wrapper
	auto descriptorPool = GetDepthPipeline()->GetDescriptorPool();

	// Create list of descriptor objects and add the objects of the model to it
	std::vector<DescriptorObject*> descriptors{ m_pUboDescriptorObject.get() };


	// Update descriptorsets
	descriptorPool->UpdateDescriptorSets(m_DepthDescriptorSets, descriptors);
}

DDM::PipelineWrapper* DDM::MeshRenderComponent::GetDepthPipeline()
{
	// Get the depth pipeline from the vulkan object and store it in static variable
	static auto pPipelineWrapper = VulkanObject::GetInstance().GetPipeline("Depth");

	// Return the depth pipeline
	return pPipelineWrapper;
}
