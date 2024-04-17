#include "MeshRenderComponent.h"
#include "VulkanRenderer.h"
#include "Material.h"
#include "TransformComponent.h"
#include "Utils.h"
#include "DescriptorPoolWrapper.h"
#include "Mesh.h"

D3D::MeshRenderComponent::MeshRenderComponent()
{
	m_pMaterial = std::make_shared<D3D::Material>();

	m_pUboDescriptorObject = std::make_unique<D3D::UboDescriptorObject<UniformBufferObject>>();
}

D3D::MeshRenderComponent::~MeshRenderComponent()
{
	
}

void D3D::MeshRenderComponent::EarlyUpdate()
{
	if (m_ShouldCreateDescriptorSets)
	{
		CreateDescriptorSets();
		m_ShouldCreateDescriptorSets = false;
	}
}

void D3D::MeshRenderComponent::SetMesh(std::shared_ptr<Mesh> pMesh)
{
	if (m_Initialized)
	{
		m_Initialized = false;
	}

	m_pMesh = pMesh;
	
	m_ShouldCreateDescriptorSets = true;

	CreateUniformBuffers();

	m_Initialized = true;
}

void D3D::MeshRenderComponent::SetMaterial(std::shared_ptr<Material> pMaterial)
{
	m_pMaterial->GetDescriptorPool()->RemoveModel(this);
	m_pMaterial = pMaterial;
	
	m_ShouldCreateDescriptorSets = true;
}

void D3D::MeshRenderComponent::Render()
{
	if (m_pMesh == nullptr)
		return;

	auto frame{ VulkanRenderer::GetInstance().GetCurrentFrame() };

	UpdateUniformBuffer(frame);

	m_pMesh->Render(GetPipeline(), &m_DescriptorSets[frame]);
}

void D3D::MeshRenderComponent::CreateUniformBuffers()
{
	// Get reference to renderer
	auto& renderer = VulkanRenderer::GetInstance();
	// Get amount of frames
	auto frames = renderer.GetMaxFrames();

	// Resize ubos to amount of frames
	m_Ubos.resize(frames);
	// Resize dirty flages to amount of frames
	m_UboChanged.resize(frames);
}

void D3D::MeshRenderComponent::CreateDescriptorSets()
{
	// Create descriptorsets
	m_pMaterial->CreateDescriptorSets(this, m_DescriptorSets);
	// Update descriptors
	UpdateDescriptorSets();
}

void D3D::MeshRenderComponent::UpdateDescriptorSets()
{
	std::vector<DescriptorObject*> descriptors{ m_pUboDescriptorObject.get() };

	// Update descriptorsets
	m_pMaterial->UpdateDescriptorSets(m_DescriptorSets, descriptors);
}

void D3D::MeshRenderComponent::UpdateUniformBuffer(uint32_t frame)
{
	auto transform{ GetTransform() };

	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), transform->GetWorldPosition());

	glm::mat4 rotationMatrix = glm::mat4_cast(transform->GetWorldRotation());

	glm::mat4 scalingMatrix = glm::scale(glm::mat4(1.0f), transform->GetWorldScale());

	m_Ubos[frame].model = translationMatrix * rotationMatrix * scalingMatrix;

	m_Ubos[frame].model[2][2] *= 1;

	VulkanRenderer::GetInstance().UpdateUniformBuffer(m_Ubos[frame]);

	m_pUboDescriptorObject->UpdateUboBuffer(m_Ubos[frame], frame);
}

D3D::PipelineWrapper* D3D::MeshRenderComponent::GetPipeline()
{
	if (m_pMaterial != nullptr)
	{
		return m_pMaterial->GetPipeline();
	}

	return VulkanRenderer::GetInstance().GetPipeline();
}