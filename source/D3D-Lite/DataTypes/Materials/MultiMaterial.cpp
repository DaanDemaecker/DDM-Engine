
#include "MultiMaterial.h"
#include "../../Vulkan/VulkanWrappers/DescriptorPoolWrapper.h"
#include "../../Includes/ImGuiIncludes.h"
#include "../../Engine/Window.h"

#include <algorithm>

D3D::MultiMaterial::MultiMaterial(const std::string& pipeline)
	:Material(pipeline)
{
	m_pMultiShaderBufferDescriptor = std::make_unique<D3D::UboDescriptorObject<MultiShaderBuffer>>();

	m_pDiffuseTextureObject = std::make_unique<D3D::TextureDescriptorObject>();

	m_pNormalTextureObject = std::make_unique<D3D::TextureDescriptorObject>();

	m_pGlossTextureObject = std::make_unique<D3D::TextureDescriptorObject>();

	m_pSpecularTextureObject = std::make_unique<D3D::TextureDescriptorObject>();

	UpdateShaderBuffer();



	auto boundCallback = std::bind(&D3D::MultiMaterial::DropFileCallback, this, std::placeholders::_1, std::placeholders::_2);

	Window::GetInstance().AddCallback(this, boundCallback);
}

D3D::MultiMaterial::~MultiMaterial()
{
	Window::GetInstance().RemoveCallback(this);
}

void D3D::MultiMaterial::OnGUI()
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Framed;

	if (ImGui::TreeNodeEx("Multi Material", flags))
	{
		ImGui::Text("Textures: ");

		DiffuseGui();

		NormalMapGui();

		GlossMapGui();

		SpecularMapGui();
		

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

void D3D::MultiMaterial::DropFileCallback(int count, const char** paths)
{
	if (count > 0)
	{
		if (m_GuiObject.diffuseHovered)
		{
			SetFileName(m_GuiObject.diffuseName, m_TextLength, paths[0]);
		}
		else if (m_GuiObject.normalMapHovered)
		{
			SetFileName(m_GuiObject.normalMapName, m_TextLength, paths[0]);
		}
		else if(m_GuiObject.glossHovered)
		{
			SetFileName(m_GuiObject.glossName, m_TextLength, paths[0]);
		}
		else if(m_GuiObject.specularHovered)
		{
			SetFileName(m_GuiObject.specularName, m_TextLength, paths[0]);
		}
	}

}

void D3D::MultiMaterial::SetFileName(char* text, int textLength, const char* path)
{
	std::fill(text, text + textLength, 0);


	for (int i{}; i < m_TextLength; i++)
	{
		if (path[i] == '\0')
		{
			break;
		}
		else
		{
			text[i] = path[i];
		}
	}
}

void D3D::MultiMaterial::DiffuseGui()
{
	bool placeHolder{ false };

	ImGui::Text("Diffuse: ");

	ImGui::InputText("Diffuse texture file path", m_GuiObject.diffuseName, IM_ARRAYSIZE(m_GuiObject.diffuseName));
	if (ImGui::IsItemHovered())
	{
		m_GuiObject.diffuseHovered = true;
	}
	else
	{
		m_GuiObject.diffuseHovered = false;
	}


	// Create a button
	if (ImGui::Button("Add diffuse Texture"))
	{
		AddDiffuseTextures({ std::string{m_GuiObject.diffuseName} });
	}

	// Create a button
	if (ImGui::Button("Clear Diffuse textures"))
	{
		m_pDiffuseTextureObject->Clear();
		m_ShouldUpdateDescriptorSets = true;
		m_MultiShaderBuffer.diffuseEnabled = false;
	}

	if (m_MultiShaderBuffer.diffuseAmount > 0)
	{
		placeHolder = static_cast<bool>(m_MultiShaderBuffer.diffuseEnabled);

		// Create a checkbox (toggle box) and update its value
		ImGui::Checkbox("Diffuse", &placeHolder);

		m_MultiShaderBuffer.diffuseEnabled = static_cast<uint32_t>(placeHolder);
	}
}

void D3D::MultiMaterial::NormalMapGui()
{

	bool placeHolder{ false };

	ImGui::Text("Normal map: ");

	ImGui::InputText("Normal map file path", m_GuiObject.normalMapName, IM_ARRAYSIZE(m_GuiObject.normalMapName));
	if (ImGui::IsItemHovered())
	{
		m_GuiObject.normalMapHovered = true;
	}
	else
	{
		m_GuiObject.normalMapHovered = false;
	}


	// Create a button
	if (ImGui::Button("Add normal map"))
	{
		AddNormalMap({ std::string{m_GuiObject.normalMapName} });
	}

	// Create a button
	if (ImGui::Button("Clear Normal maps"))
	{
		m_pNormalTextureObject->Clear();
		m_ShouldUpdateDescriptorSets = true;
		m_MultiShaderBuffer.normalEnabled = false;
	}

	if (m_MultiShaderBuffer.normalAmount > 0)
	{
		placeHolder = static_cast<bool>(m_MultiShaderBuffer.normalEnabled);

		// Create a checkbox (toggle box) and update its value
		ImGui::Checkbox("Normal map", &placeHolder);

		m_MultiShaderBuffer.normalEnabled = static_cast<uint32_t>(placeHolder);
	}
}

void D3D::MultiMaterial::GlossMapGui()
{
	bool placeHolder{ false };

	ImGui::Text("Gloss map: ");

	ImGui::InputText("Gloss map file path", m_GuiObject.glossName, IM_ARRAYSIZE(m_GuiObject.glossName));
	if (ImGui::IsItemHovered())
	{
		m_GuiObject.glossHovered = true;
	}
	else
	{
		m_GuiObject.glossHovered = false;
	}


	// Create a button
	if (ImGui::Button("Add gloss map"))
	{
		AddGlossTextures({ std::string{m_GuiObject.glossName} });
	}

	// Create a button
	if (ImGui::Button("Clear Gloss maps"))
	{
		m_pGlossTextureObject->Clear();
		m_ShouldUpdateDescriptorSets = true;
		m_MultiShaderBuffer.glossEnabled = false;
	}

	if (m_MultiShaderBuffer.glossAmount > 0)
	{
		placeHolder = static_cast<bool>(m_MultiShaderBuffer.glossEnabled);

		// Create a checkbox (toggle box) and update its value
		ImGui::Checkbox("Gloss map", &placeHolder);

		m_MultiShaderBuffer.glossEnabled = static_cast<uint32_t>(placeHolder);
	}
}

void D3D::MultiMaterial::SpecularMapGui()
{
	bool placeHolder{ false };

	ImGui::Text("Specular map: ");
	ImGui::InputText("Specular map file path", m_GuiObject.specularName, IM_ARRAYSIZE(m_GuiObject.specularName));
	if (ImGui::IsItemHovered())
	{
		m_GuiObject.specularHovered = true;
	}
	else
	{
		m_GuiObject.specularHovered = false;
	}


	// Create a button
	if (ImGui::Button("Add specular map"))
	{
		AddSpecularTextures({ std::string{m_GuiObject.specularName} });
	}

	// Create a button
	if (ImGui::Button("Clear specular maps"))
	{
		m_pSpecularTextureObject->Clear();
		m_ShouldUpdateDescriptorSets = true;
		m_MultiShaderBuffer.glossEnabled = false;
	}

	if (m_MultiShaderBuffer.specularAmount > 0)
	{
		placeHolder = static_cast<bool>(m_MultiShaderBuffer.specularEnabled);

		// Create a checkbox (toggle box) and update its value
		ImGui::Checkbox("Specular map", &placeHolder);

		m_MultiShaderBuffer.specularEnabled = static_cast<uint32_t>(placeHolder);
	}

}
