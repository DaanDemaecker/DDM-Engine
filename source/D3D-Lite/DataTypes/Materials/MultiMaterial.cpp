
#include "MultiMaterial.h"
#include "../../Vulkan/VulkanWrappers/DescriptorPoolWrapper.h"
#include "../../Includes/ImGuiIncludes.h"

D3D::MultiMaterial::MultiMaterial(const std::string& pipeline)
	:Material(pipeline)
{
	m_pMultiShaderBufferDescriptor = std::make_unique<D3D::UboDescriptorObject<MultiShaderBuffer>>();

	m_pDiffuseTextureObject = std::make_unique<D3D::TextureDescriptorObject>();

	m_pNormalTextureObject = std::make_unique<D3D::TextureDescriptorObject>();

	m_pGlossTextureObject = std::make_unique<D3D::TextureDescriptorObject>();

	m_pSpecularTextureObject = std::make_unique<D3D::TextureDescriptorObject>();

	UpdateShaderBuffer();
}

void D3D::MultiMaterial::OnGUI()
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Framed;

	if (ImGui::TreeNodeEx("Multi Material", flags))
	{
		ImGui::Text("Textures: ");

		bool placeHolder{ false };

		ImGui::Text("Diffuse: ");

		ImGui::InputText("File path", m_GuiObject.diffuseName, IM_ARRAYSIZE(m_GuiObject.diffuseName));

		// Create a button
		if (ImGui::Button("Add Texture"))
		{
			AddDiffuseTextures({ std::string{m_GuiObject.diffuseName} });
		}

		// Create a button
		if (ImGui::Button("Clear Textures"))
		{
			m_pDiffuseTextureObject->Clear();
			m_ShouldUpdateDescriptorSets = true;
			m_MultiShaderBuffer.diffuseEnabled = false;
		}

		if (m_MultiShaderBuffer.diffuseAmount > 0)
		{
			placeHolder = static_cast<bool>(m_MultiShaderBuffer.diffuseEnabled);

			// Create a checkbox (toggle box) and update its value
			ImGui::Checkbox("", &placeHolder);

			m_MultiShaderBuffer.diffuseEnabled = static_cast<uint32_t>(placeHolder);
		}


		ImGui::Text("Normal map: ");
		if (m_MultiShaderBuffer.normalAmount > 0)
		{
			placeHolder = static_cast<bool>(m_MultiShaderBuffer.normalEnabled);

			// Create a checkbox (toggle box) and update its value
			ImGui::Checkbox("", &placeHolder);

			m_MultiShaderBuffer.normalEnabled = static_cast<uint32_t>(placeHolder);
		}


		ImGui::Text("Gloss map: ");
		if (m_MultiShaderBuffer.glossAmount > 0)
		{
			placeHolder = static_cast<bool>(m_MultiShaderBuffer.glossEnabled);

			// Create a checkbox (toggle box) and update its value
			ImGui::Checkbox("", &placeHolder);

			m_MultiShaderBuffer.glossEnabled = static_cast<uint32_t>(placeHolder);
		}


		ImGui::Text("Dpecular map: ");
		if (m_MultiShaderBuffer.specularAmount > 0)
		{
			placeHolder = static_cast<bool>(m_MultiShaderBuffer.specularEnabled);

			// Create a checkbox (toggle box) and update its value
			ImGui::Checkbox("", &placeHolder);

			m_MultiShaderBuffer.specularEnabled = static_cast<uint32_t>(placeHolder);
		}


		ImGui::TreePop();
	}

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

	descriptorObjectList.push_back(m_pNormalTextureObject.get());

	descriptorObjectList.push_back(m_pGlossTextureObject.get());

	descriptorObjectList.push_back(m_pSpecularTextureObject.get());

	// Update descriptorsets
	descriptorPool->UpdateDescriptorSets(descriptorSets, descriptorObjectList);

	m_ShouldUpdateDescriptorSets = false;
}

void D3D::MultiMaterial::AddDiffuseTextures(std::initializer_list<const std::string>&& filePaths)
{
	AddDiffuseTextures(filePaths);
}

void D3D::MultiMaterial::AddDiffuseTextures(std::initializer_list<const std::string>& filePaths)
{
	m_pDiffuseTextureObject->AddTextures(filePaths);
	
	m_MultiShaderBuffer.diffuseAmount = m_pDiffuseTextureObject->GetTextureAmount();
	m_MultiShaderBuffer.diffuseEnabled = true;

	m_ShouldUpdateDescriptorSets = true;

	UpdateShaderBuffer();
}

void D3D::MultiMaterial::AddNormalMap(std::initializer_list<const std::string>&& filePaths)
{
	AddNormalMap(filePaths);
}

void D3D::MultiMaterial::AddNormalMap(std::initializer_list<const std::string>& filePaths)
{
	m_pNormalTextureObject->AddTextures(filePaths);

	m_MultiShaderBuffer.normalAmount = m_pNormalTextureObject->GetTextureAmount();
	m_MultiShaderBuffer.normalEnabled = true;

	m_ShouldUpdateDescriptorSets = true;

	UpdateShaderBuffer();
}

void D3D::MultiMaterial::AddGlossTextures(std::initializer_list<const std::string>&& filePaths)
{
	AddGlossTextures(filePaths);
}

void D3D::MultiMaterial::AddGlossTextures(std::initializer_list<const std::string>& filePaths)
{
	m_pGlossTextureObject->AddTextures(filePaths);

	m_MultiShaderBuffer.glossAmount = m_pGlossTextureObject->GetTextureAmount();
	m_MultiShaderBuffer.glossEnabled = true;

	m_ShouldUpdateDescriptorSets = true;

	UpdateShaderBuffer();
}

void D3D::MultiMaterial::AddSpecularTextures(std::initializer_list<const std::string>&& filePaths)
{
	AddSpecularTextures(filePaths);
}

void D3D::MultiMaterial::AddSpecularTextures(std::initializer_list<const std::string>& filePaths)
{
	m_pSpecularTextureObject->AddTextures(filePaths);

	m_MultiShaderBuffer.specularAmount = m_pSpecularTextureObject->GetTextureAmount();
	m_MultiShaderBuffer.specularEnabled = true;

	m_ShouldUpdateDescriptorSets = true;

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
