#include "ResourceManager.h"

std::shared_ptr<D3D::Mesh> D3D::ResourceManager::LoadMesh(std::string&& filePath)
{
	return LoadMesh(filePath);
}

std::shared_ptr<D3D::Mesh> D3D::ResourceManager::LoadMesh(std::string& filePath)
{
	std::shared_ptr<D3D::Mesh> pMesh{};

	pMesh = std::make_shared<D3D::Mesh>(filePath);

	return pMesh;
}
