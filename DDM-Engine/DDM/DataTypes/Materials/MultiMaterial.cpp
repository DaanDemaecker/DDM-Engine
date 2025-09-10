// MultiMaterial.cpp

// Header include
#include "MultiMaterial.h"
// File includes
#include "Vulkan/VulkanWrappers/DescriptorPoolWrapper.h"

#include "Includes/ImGuiIncludes.h"

#include "Engine/Window.h"

// Standard library includes
#include <algorithm>

DDM::MultiMaterial::MultiMaterial()
	:Material("MultiShader")
{
	// Create descriptor objects and initialize
	m_pMultiShaderBufferDescriptor = std::make_unique<DDM::UboDescriptorObject<MultiShaderBuffer>>();
	
	UpdateShaderBuffer();

	// Initialize texture derscriptor objects
	for (int i{}; i <= mm_Last; ++i)
	{
		m_pTextureObjects.push_back(std::make_unique<DDM::TextureDescriptorObject>());
	}

	// Bind the callback for the file dropping
	auto boundCallback = std::bind(&DDM::MultiMaterial::DropFileCallback, this, std::placeholders::_1, std::placeholders::_2);
	Window::GetInstance().AddCallback(this, boundCallback);
}

DDM::MultiMaterial::~MultiMaterial()
{
	// Remove the callback
	Window::GetInstance().RemoveCallback(this);
}

void DDM::MultiMaterial::OnGUI()
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Framed;

	
	

	// Start an ImGui tree
	if (ImGui::TreeNodeEx("Multi Material", flags))
	{
		ImGui::Text("Textures: ");

		// Handle all texture gui's seperately
		DiffuseGui();

		NormalMapGui();

		GlossMapGui();

		SpecularMapGui();
		
		// End the tree
		ImGui::TreePop();
	}
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

	// Add descriptor object of multishaderbuffer
	descriptorObjectList.push_back(m_pMultiShaderBufferDescriptor.get());

	// Add descriptor objects of all textures
	for (auto& descriptorObject : m_pTextureObjects)
	{
		descriptorObjectList.push_back(descriptorObject.get());
	}

	// Update descriptorsets
	descriptorPool->UpdateDescriptorSets(descriptorSets, descriptorObjectList);

	// Indicate that descriptor sets are updated
	m_ShouldUpdateDescriptorSets = false;
}

void DDM::MultiMaterial::AddDiffuseTexture(std::string& filePath)
{
	// Add texture to the descriptor object
	m_pTextureObjects[mm_Diffuse]->AddTexture(filePath);

	// Recount the amount of diffuse textures and enable diffuse texture
	m_MultiShaderBuffer.diffuseAmount = static_cast<int>(m_pTextureObjects[mm_Diffuse]->GetTextureAmount());
	m_MultiShaderBuffer.diffuseEnabled = true;

	// Update shader buffer
	UpdateShaderBuffer();
}

void DDM::MultiMaterial::AddDiffuseTexture(std::string&& filePath)
{
	// Propagate to lvalue overloaded function
	AddDiffuseTexture(filePath);
}

void DDM::MultiMaterial::AddNormalMap(std::string& filePath)
{
	// Add texture to the descriptor object
	m_pTextureObjects[mm_Normal]->AddTexture(filePath);

	// Recount the amount of diffuse textures and enable diffuse texture
	m_MultiShaderBuffer.normalAmount = static_cast<int>(m_pTextureObjects[mm_Normal]->GetTextureAmount());
	m_MultiShaderBuffer.normalEnabled = true;

	// Update shader buffer
	UpdateShaderBuffer();
}

void DDM::MultiMaterial::AddNormalMap(std::string&& filePath)
{
	// Propagate to lvalue overloaded function
	AddNormalMap(filePath);
}

void DDM::MultiMaterial::AddGlossTexture(std::string& filePath)
{
	// Add texture to the descriptor object
	m_pTextureObjects[mm_Gloss]->AddTexture(filePath);

	// Recount the amount of diffuse textures and enable diffuse texture
	m_MultiShaderBuffer.glossAmount = static_cast<int>(m_pTextureObjects[mm_Gloss]->GetTextureAmount());
	m_MultiShaderBuffer.glossEnabled = true;

	// Update shader buffer
	UpdateShaderBuffer();
}

void DDM::MultiMaterial::AddGlossTexture(std::string&& filePath)
{
	// Propagate to lvalue overloaded function
	AddGlossTexture(filePath);
}

void DDM::MultiMaterial::AddSpecularTexture(std::string& filePath)
{
	// Add texture to the descriptor object
	m_pTextureObjects[mm_Specular]->AddTexture(filePath);

	// Recount the amount of diffuse textures and enable diffuse texture
	m_MultiShaderBuffer.specularAmount = static_cast<int>(m_pTextureObjects[mm_Specular]->GetTextureAmount());
	m_MultiShaderBuffer.specularEnabled = true;

	// Update shader buffer
	UpdateShaderBuffer();
}

void DDM::MultiMaterial::AddSpecularTexture(std::string&& filePath)
{
	// Propagate to lvalue overloaded function
	AddSpecularTexture(filePath);
}

void DDM::MultiMaterial::UpdateShaderBuffer()
{
	// Get max amount of frames and loop through them
	auto maxFrames = DDM::VulkanObject::GetInstance().GetMaxFrames();
	for (int frame{}; frame < maxFrames; frame++)
	{
		// Update descriptor object for this frame
		m_pMultiShaderBufferDescriptor->UpdateUboBuffer(&m_MultiShaderBuffer, frame);
	}

	// Indicate that descriptorsets need to be updated
	m_ShouldUpdateDescriptorSets = true;
}

void DDM::MultiMaterial::DropFileCallback(int count, const char** paths)
{
	// If no files, return
	if (count == 0)
		return;

	// Check which box is hovered and propagate t correct function
	if (m_GuiObject.diffuseHovered)
	{
		SetFileName(m_GuiObject.diffuseName, m_TextLength, paths[0]);
	}
	else if (m_GuiObject.normalMapHovered)
	{
		SetFileName(m_GuiObject.normalMapName, m_TextLength, paths[0]);
	}
	else if (m_GuiObject.glossHovered)
	{
		SetFileName(m_GuiObject.glossName, m_TextLength, paths[0]);
	}
	else if (m_GuiObject.specularHovered)
	{
		SetFileName(m_GuiObject.specularName, m_TextLength, paths[0]);
	}

}

void DDM::MultiMaterial::SetFileName(char* text, int textLength, const char* path)
{
	// Clear the text
	std::fill(text, text + textLength, 0);

	// Loop through path characters and fill in the text, if escape character is hit, break
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
	// Set textblock
	ImGui::Text("Diffuse: ");

	// Input text for file path
	ImGui::InputText("Diffuse texture file path", m_GuiObject.diffuseName, m_TextLength);
	
	// Check if input text is hovered
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
		// If button is pressed, add diffuse texture
		AddDiffuseTexture( std::string{m_GuiObject.diffuseName});
		// Clear text
		std::fill(m_GuiObject.diffuseName, m_GuiObject.diffuseName + m_TextLength, 0);
	}

	// Create a button
	if (ImGui::Button("Clear Diffuse textures"))
	{
		// If buttons is pressed, clear 
		m_pTextureObjects[mm_Diffuse]->Clear();
		m_MultiShaderBuffer.diffuseEnabled = false;
		m_MultiShaderBuffer.diffuseAmount = 0;

		// Update shader buffer
		UpdateShaderBuffer();
	}

	if (m_MultiShaderBuffer.diffuseAmount > 0)
	{
		// Initialize placeholder boolean
		bool placeHolder{ static_cast<bool>(m_MultiShaderBuffer.diffuseEnabled) };

		// Create a checkbox (toggle box) and update its value
		ImGui::Checkbox("Diffuse", &placeHolder);

		// If value changed, update shaderbuffer
		if (placeHolder != static_cast<bool>(m_MultiShaderBuffer.diffuseEnabled))
		{
			m_MultiShaderBuffer.diffuseEnabled = static_cast<uint32_t>(placeHolder);
			UpdateShaderBuffer();
		}
	}
}

void DDM::MultiMaterial::NormalMapGui()
{
	// Set textblock
	ImGui::Text("Normal map: ");

	// Input text for file path
	ImGui::InputText("Normal texture file path", m_GuiObject.normalMapName, m_TextLength);

	// Check if input text is hovered
	if (ImGui::IsItemHovered())
	{
		m_GuiObject.normalMapHovered = true;
	}
	else
	{
		m_GuiObject.normalMapHovered = false;
	}


	// Create a button
	if (ImGui::Button("Add normal Texture"))
	{
		// If button is pressed, add normal texture
		AddNormalMap(std::string{ m_GuiObject.normalMapName });
		// Clear text
		std::fill(m_GuiObject.normalMapName, m_GuiObject.normalMapName + m_TextLength, 0);
	}

	// Create a button
	if (ImGui::Button("Clear Normal textures"))
	{
		// If buttons is pressed, clear 
		m_pTextureObjects[mm_Normal]->Clear();
		m_MultiShaderBuffer.normalEnabled = false;
		m_MultiShaderBuffer.normalAmount = 0;

		// Update shader buffer
		UpdateShaderBuffer();
	}

	if (m_MultiShaderBuffer.normalAmount > 0)
	{
		// Initialize placeholder boolean
		bool placeHolder{ static_cast<bool>(m_MultiShaderBuffer.normalEnabled) };

		// Create a checkbox (toggle box) and update its value
		ImGui::Checkbox("Normal", &placeHolder);

		// If value changed, update shaderbuffer
		if (placeHolder != static_cast<bool>(m_MultiShaderBuffer.normalEnabled))
		{
			m_MultiShaderBuffer.normalEnabled = static_cast<uint32_t>(placeHolder);
			UpdateShaderBuffer();
		}
	}
}

void DDM::MultiMaterial::GlossMapGui()
{
	// Set textblock
	ImGui::Text("Gloss: ");

	// Input text for file path
	ImGui::InputText("Gloss texture file path", m_GuiObject.glossName, m_TextLength);

	// Check if input text is hovered
	if (ImGui::IsItemHovered())
	{
		m_GuiObject.glossHovered = true;
	}
	else
	{
		m_GuiObject.glossHovered = false;
	}


	// Create a button
	if (ImGui::Button("Add gloss Texture"))
	{
		// If button is pressed, add gloss texture
		AddGlossTexture(std::string{ m_GuiObject.glossName });
		// Clear text
		std::fill(m_GuiObject.glossName, m_GuiObject.glossName + m_TextLength, 0);
	}

	// Create a button
	if (ImGui::Button("Clear gloss textures"))
	{
		// If buttons is pressed, clear 
		m_pTextureObjects[mm_Gloss]->Clear();
		m_MultiShaderBuffer.glossEnabled = false;
		m_MultiShaderBuffer.glossAmount = 0;

		// Update shader buffer
		UpdateShaderBuffer();
	}

	if (m_MultiShaderBuffer.glossAmount > 0)
	{
		// Initialize placeholder boolean
		bool placeHolder{ static_cast<bool>(m_MultiShaderBuffer.glossEnabled) };

		// Create a checkbox (toggle box) and update its value
		ImGui::Checkbox("Gloss", &placeHolder);

		// If value changed, update shaderbuffer
		if (placeHolder != static_cast<bool>(m_MultiShaderBuffer.glossEnabled))
		{
			m_MultiShaderBuffer.glossEnabled = static_cast<uint32_t>(placeHolder);
			UpdateShaderBuffer();
		}
	}
}

void DDM::MultiMaterial::SpecularMapGui()
{
	// Set textblock
	ImGui::Text("Specular: ");

	// Input text for file path
	ImGui::InputText("Specular texture file path", m_GuiObject.specularName, m_TextLength);

	// Check if input text is hovered
	if (ImGui::IsItemHovered())
	{
		m_GuiObject.diffuseHovered = true;
	}
	else
	{
		m_GuiObject.diffuseHovered = false;
	}


	// Create a button
	if (ImGui::Button("Add Specular Texture"))
	{
		// If button is pressed, add specular texture
		AddSpecularTexture(std::string{ m_GuiObject.specularName });
		// Clear text
		std::fill(m_GuiObject.specularName, m_GuiObject.specularName + m_TextLength, 0);
	}

	// Create a button
	if (ImGui::Button("Clear Specular textures"))
	{
		// If buttons is pressed, clear 
		m_pTextureObjects[mm_Specular]->Clear();
		m_MultiShaderBuffer.specularEnabled = false;
		m_MultiShaderBuffer.specularAmount = 0;

		// Update shader buffer
		UpdateShaderBuffer();
	}

	if (m_MultiShaderBuffer.specularAmount > 0)
	{
		// Initialize placeholder boolean
		bool placeHolder{ static_cast<bool>(m_MultiShaderBuffer.specularEnabled) };

		// Create a checkbox (toggle box) and update its value
		ImGui::Checkbox("Diffuse", &placeHolder);

		// If value changed, update shaderbuffer
		if (placeHolder != static_cast<bool>(m_MultiShaderBuffer.specularEnabled))
		{
			m_MultiShaderBuffer.specularEnabled = static_cast<uint32_t>(placeHolder);
			UpdateShaderBuffer();
		}
	}

}
