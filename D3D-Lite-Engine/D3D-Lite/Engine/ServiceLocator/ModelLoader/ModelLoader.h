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
#include <memory>

namespace DDM3
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
	virtual void LoadModel(const std::string& path, std::vector<DDM3::Vertex>& vertices, std::vector<uint32_t>& indices) = 0;

	// Load in animations given a file path
	// Parameters:
	//     - path: The path to the file
	//     - animationClips: The vector that will be used to store the animationc lips
	virtual void LoadAnimations(const std::string& path, std::vector<std::unique_ptr<AnimationClip>>& animationClips) = 0;
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
	virtual void LoadModel(const std::string& /*path*/, std::vector<DDM3::Vertex>& /*vertices*/, std::vector<uint32_t>& /*indices*/) override {};

	// Load in animations given a file path
	// Parameters:
	//     - path: The path to the file
	//     - animationClips: The vector that will be used to store the animationc lips
	virtual void LoadAnimations(const std::string& path, std::vector<std::unique_ptr<AnimationClip>>& animationClips) override {};

	};
}

#endif // !ModelLoaderIncluded