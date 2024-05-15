
#include "MultiMaterial.h"
#include "../DescriptorObjects/TextureDescriptorObject.h"
#include "../DescriptorObjects/UboDescriptorObject.h"
#include "../../Vulkan/VulkanWrappers/DescriptorPoolWrapper.h"
#include "../../Includes/ImGuiIncludes.h"

D3D::MultiMaterial::MultiMaterial(const std::string& pipeline)
	:Material(pipeline)
{
	m_pMultiShaderBufferDescriptor = std::make_unique<D3D::UboDescriptorObject<MultiShaderBuffer>>();

	m_pDiffuseTextureObject = std::make_unique<D3D::TextureDescriptorObject>();

	UpdateShaderBuffer();
}

void D3D::MultiMaterial::OnGUI()
{
	ImGui::Begin("MultiMaterial", &m_ShowGuiWindow);

	ImGui::Text("Textures: ");
	
	bool placeHolder{static_cast<bool>(m_MultiShaderBuffer.diffuseEnabled)};

	// Create a checkbox (toggle box) and update its value
	ImGui::Checkbox("Diffuse", &placeHolder);

	m_MultiShaderBuffer.diffuseEnabled = static_cast<uint32_t>(placeHolder);

	ImGui::End();

	UpdateShaderBuffer();
}

void D3D::MultiMaterial::UpdateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, std::vector<DescriptorObject*>& descriptorObjects)
{
	// Get pointer to the descriptorpool wrapper
	auto descriptorPool = GetDescriptorPool();

	// Create list of descriptor objects and add the objects of the model to it
	std::vector<DescriptorObject*> descriptorObjectList{};

	for (auto& descriptorObject : descriptorObjects)
	{
		descriptorObjectList.push_back(descriptorObject);
	}

	// Add the descriptor object of the global light
	descriptorObjectList.push_back(VulkanRenderer::GetInstance().GetLightDescriptor());

	descriptorObjectList.push_back(m_pMultiShaderBufferDescriptor.get());

	descriptorObjectList.push_back(m_pDiffuseTextureObject.get());

	// Update descriptorsets
	descriptorPool->UpdateDescriptorSets(descriptorSets, descriptorObjectList);
}

void D3D::MultiMaterial::AddDiffuseTextures(std::initializer_list<const std::string>&& filePaths)
{
	AddDiffuseTextures(filePaths);
}

void D3D::MultiMaterial::AddDiffuseTextures(std::initializer_list<const std::string>& filePaths)
{
	m_pDiffuseTextureObject = std::make_unique<D3D::TextureDescriptorObject>(filePaths);
	
	m_MultiShaderBuffer.diffuseAmount = static_cast<int>(filePaths.size());
	m_MultiShaderBuffer.diffuseEnabled = true;
	
	UpdateShaderBuffer();
}

void D3D::MultiMaterial::UpdateShaderBuffer()
{
	auto& renderer{ D3D::VulkanRenderer::GetInstance() };

	for (int frame{}; frame < renderer.GetMaxFrames(); frame++)
	{
		m_pMultiShaderBufferDescriptor->UpdateUboBuffer(m_MultiShaderBuffer, frame);
	}
}
