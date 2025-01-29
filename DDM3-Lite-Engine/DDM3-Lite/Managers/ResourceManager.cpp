// ResourceManager.cpp

// Header include
#include "ResourceManager.h"

// File includes
#include "Managers/ConfigManager.h"

#include "Vulkan/VulkanRenderer.h"

std::shared_ptr<DDM3::Mesh> DDM3::ResourceManager::LoadMesh(std::string&& filePath)
{
	return LoadMesh(filePath);
}

std::shared_ptr<DDM3::Mesh> DDM3::ResourceManager::LoadMesh(std::string& filePath)
{
	std::shared_ptr<DDM3::Mesh> pMesh{};

	try
	{
		pMesh = CreateMesh(filePath);

		return pMesh;
	}
	catch(const std::exception& e)
	{
		std::cerr << "Exception caught: " << e.what() << std::endl;
		return m_pDefaultMesh;
	}
}

DDM3::ResourceManager::ResourceManager()
{
	m_pDefaultMesh = CreateMesh(ConfigManager::GetInstance().GetString("DefaultModelName"));
}

std::shared_ptr<DDM3::Mesh> DDM3::ResourceManager::CreateMesh(const std::string& filePath)
{
	return std::shared_ptr<DDM3::Mesh>(new DDM3::Mesh(filePath));
}
