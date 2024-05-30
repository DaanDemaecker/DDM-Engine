#include "ResourceManager.h"
#include "../Vulkan/VulkanRenderer.h"
#include "../Managers/ConfigManager.h"

std::shared_ptr<D3D::Mesh> D3D::ResourceManager::LoadMesh(std::string&& filePath)
{
	return LoadMesh(filePath);
}

std::shared_ptr<D3D::Mesh> D3D::ResourceManager::LoadMesh(std::string& filePath)
{
	std::shared_ptr<D3D::Mesh> pMesh{};

	try
	{
		pMesh = std::make_shared<D3D::Mesh>(filePath);

		return pMesh;
	}
	catch(const std::exception& e)
	{
		std::cerr << "Exception caught: " << e.what() << std::endl;
		return m_pDefaultMesh;
	}
}

D3D::ResourceManager::ResourceManager()
{
	m_pDefaultMesh = std::make_shared<D3D::Mesh>("Resources/DefaultResources/error.obj");
}
