// ModelLoader.h
// This class will serve as the baseclass of all model loaders
// This class will be an abstract class as an instance of the class shouldn't be allowed
// This file will also hold the default model loader class

#ifndef ModelLoaderIncluded
#define ModelLoaderIncluded

// File includes
#include "../../../DataTypes/Structs.h"

// Standard library includes
#include <string>

namespace D3D
{
	class ModelLoader
	{
	public:
		ModelLoader() = default;
		virtual ~ModelLoader() = default;

		ModelLoader(ModelLoader& other) = delete;
		ModelLoader(ModelLoader&& other) = delete;

		ModelLoader& operator=(ModelLoader& other) = delete;
		ModelLoader& operator=(ModelLoader&& other) = delete;

	// Load in a 3D model given a file path
	// Parameters:
	//     - path: The path to the model file
	//     - vertices: The vector that will be used to store the vertices
	//     - indices: The vector that will be used to store the indices
	virtual void LoadModel(const std::string& path, std::vector<D3D::Vertex>& vertices, std::vector<uint32_t>& indices) = 0;

	};


	class DefaultModelLoader final : public ModelLoader
	{
	public:
		DefaultModelLoader() = default;
		virtual ~DefaultModelLoader() override = default;

	// Load in a 3D model given a file path
	// Parameters:
	//     - path: The path to the model file
	//     - vertices: The vector that will be used to store the vertices
	//     - indices: The vector that will be used to store the indices
	virtual void LoadModel(const std::string& /*path*/, std::vector<D3D::Vertex>& /*vertices*/, std::vector<uint32_t>& /*indices*/) override {};

	};
}

#endif // !ModelLoaderIncluded