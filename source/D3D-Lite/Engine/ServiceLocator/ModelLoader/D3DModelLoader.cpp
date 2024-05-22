// D3DModelLoader.cpp

// File includes
#include "D3DModelLoader.h"
#include "ObjLoader.h"
#include "FbxLoader.h"
#include "../../../Utils/Utils.h"

// Standard library includes
#include <stdexcept>
#include <iostream>

D3D::D3DModelLoader::D3DModelLoader()
{
	m_pObjLoader = std::make_unique<D3D::ObjLoader>();

	m_pFbxLoader = std::make_unique<D3D::FbxLoader>();
}

D3D::D3DModelLoader::~D3DModelLoader()
{

}

void D3D::D3DModelLoader::LoadModel(const std::string& path, std::vector<D3D::Vertex>& vertices, std::vector<uint32_t>& indices)
{
	// Get the extension of the file
	auto extension{std::move(Utils::GetExtension(path))};

	vertices.clear();
	indices.clear();

	if (extension == "obj")
	{
		// If the extension is .obj, use obj loader
		m_pObjLoader->LoadObj(path, vertices, indices);
	}
	else if (extension == "fbx")
	{
		// If the extension is .fbx, use fbx sdk to load in model
		m_pFbxLoader->LoadFbx(path, vertices, indices);
	}
	else
	{
		// If the extension can't be handled, warn user
		std::cout << extension << " is not a supported model format \n";
	}
}

