
#include "MultiMaterial.h"
#include "../../Vulkan/VulkanWrappers/DescriptorPoolWrapper.h"
#include "../../Includes/ImGuiIncludes.h"
#include "../../Engine/Window.h"

#include <algorithm>

DDM::MultiMaterial::MultiMaterial()
	:Material("MultiShader")
{
	m_pMultiShaderBufferDescriptor = std::make_unique<DDM::UboDescriptorObject<MultiShaderBuffer>>();

	m_pDiffuseTextureObject = std::make_unique<DDM::TextureDescriptorObject>();

	m_pNormalTextureObject = std::make_unique<DDM::TextureDescriptorObject>();

	m_pGlossTextureObject = std::make_unique<DDM::TextureDescriptorObject>();

	m_pSpecularTextureObject = std::make_unique<DDM::TextureDescriptorObject>();

	UpdateShaderBuffer();



	auto boundCallback = std::bind(&DDM::MultiMaterial::DropFileCallback, this, std::placeholders::_1, std::placeholders::_2);

	Window::GetInstance().AddCallback(this, boundCallback);
}

DDM::MultiMaterial::~MultiMaterial()
{
	Window::GetInstance().RemoveCallback(this);
}

void DDM::MultiMaterial::OnGUI()
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

void DDM::MultiMaterial::UpdateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, std::vector<DescriptorObject*>& descriptorObjects)
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
	descriptorObjectList.push_back(VulkanObject::GetInstance().GetLightDescriptor());

	descriptorObjectList.push_back(m_pMultiShaderBufferDescriptor.get());

	descriptorObjectList.push_back(m_pDiffuseTextureObject.get());

	descriptorObjectList.push_back(m_pNormalTextureObject.get());

	descriptorObjectList.push_back(m_pGlossTextureObject.get());

	descriptorObjectList.push_back(m_pSpecularTextureObject.get());

	// Update descriptorsets
	descriptorPool->UpdateDescriptorSets(descriptorSets, descriptorObjectList);

	m_ShouldUpdateDescriptorSets = false;
}

void DDM::MultiMaterial::AddDiffuseTexture(std::string& filePath)
{
	m_pDiffuseTextureObject->AddTexture(filePath);

	m_MultiShaderBuffer.diffuseAmount = m_pDiffuseTextureObject->GetTextureAmount();
	m_MultiShaderBuffer.diffuseEnabled = true;

	m_ShouldUpdateDescriptorSets = true;

	UpdateShaderBuffer();
}

void DDM::MultiMaterial::AddDiffuseTexture(std::string&& filePath)
{
	AddDiffuseTexture(filePath);
}

void DDM::MultiMaterial::AddNormalMap(std::string& filePath)
{
	m_pNormalTextureObject->AddTexture(filePath);

	m_MultiShaderBuffer.normalAmount = m_pNormalTextureObject->GetTextureAmount();
	m_MultiShaderBuffer.normalEnabled = true;

	m_ShouldUpdateDescriptorSets = true;

	UpdateShaderBuffer();
}

void DDM::MultiMaterial::AddNormalMap(std::string&& filePath)
{
	AddNormalMap(filePath);
}

void DDM::MultiMaterial::AddGlossTextures(std::initializer_list<const std::string>&& filePaths)
{
	AddGlossTextures(filePaths);
}

void DDM::MultiMaterial::AddGlossTextures(std::initializer_list<const std::string>& filePaths)
{
	m_pGlossTextureObject->AddTextures(filePaths);

	m_MultiShaderBuffer.glossAmount = m_pGlossTextureObject->GetTextureAmount();
	m_MultiShaderBuffer.glossEnabled = true;

	m_ShouldUpdateDescriptorSets = true;

	UpdateShaderBuffer();
}

void DDM::MultiMaterial::AddSpecularTextures(std::initializer_list<const std::string>&& filePaths)
{
	AddSpecularTextures(filePaths);
}

void DDM::MultiMaterial::AddSpecularTextures(std::initializer_list<const std::string>& filePaths)
{
	m_pSpecularTextureObject->AddTextures(filePaths);

	m_MultiShaderBuffer.specularAmount = m_pSpecularTextureObject->GetTextureAmount();
	m_MultiShaderBuffer.specularEnabled = true;

	m_ShouldUpdateDescriptorSets = true;

	UpdateShaderBuffer();
}

void DDM::MultiMaterial::UpdateShaderBuffer()
{
	auto& renderer{ DDM::VulkanObject::GetInstance() };

	for (int frame{}; frame < renderer.GetMaxFrames(); frame++)
	{
		m_pMultiShaderBufferDescriptor->UpdateUboBuffer(&m_MultiShaderBuffer, frame);
	}
}

void DDM::MultiMaterial::DropFileCallback(int count, const char** paths)
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

void DDM::MultiMaterial::SetFileName(char* text, int textLength, const char* path)
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

void DDM::MultiMaterial::DiffuseGui()
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
		AddDiffuseTexture( std::string{m_GuiObject.diffuseName});
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

void DDM::MultiMaterial::NormalMapGui()
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

void DDM::MultiMaterial::GlossMapGui()
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

void DDM::MultiMaterial::SpecularMapGui()
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
