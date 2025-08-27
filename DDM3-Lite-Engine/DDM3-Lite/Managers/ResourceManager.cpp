// ResourceManager.cpp

// Header include
#include "ResourceManager.h"

// File includes
#include "Managers/ConfigManager.h"

#include "Vulkan/VulkanObject.h"

std::shared_ptr<DDM::Mesh> DDM::ResourceManager::LoadMesh(std::string&& filePath)
{
	return LoadMesh(filePath);
}

std::shared_ptr<DDM::Mesh> DDM::ResourceManager::LoadMesh(std::string& filePath)
{
	std::shared_ptr<DDM::Mesh> pMesh{};

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

DDM::ResourceManager::ResourceManager()
{
	m_pDefaultMesh = CreateMesh(ConfigManager::GetInstance().GetString("DefaultModelName"));
}

std::shared_ptr<DDM::Mesh> DDM::ResourceManager::CreateMesh(const std::string& filePath)
{
	return std::shared_ptr<DDM::Mesh>(new DDM::Mesh(filePath));
}
