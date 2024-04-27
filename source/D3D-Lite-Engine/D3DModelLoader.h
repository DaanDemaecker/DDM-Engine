// D3DModelLoader.h
// This class is the standard model loader, users can replace it by registering another model loader to the service locator

#ifndef D3DModelLoaderIncluded
#define D3DModelLoaderIncluded

// File includes
#include "ModelLoader.h"

namespace D3D
{
	class D3DModelLoader final : public ModelLoader
	{
	public:
		D3DModelLoader() = default;
		virtual ~D3DModelLoader() override = default;

		// Load in a 3D model given a file path
		// Parameters:
		//     - path: The path to the model file
		//     - vertices: The vector that will be used to store the vertices
		//     - indices: The vector that will be used to store the indices
		virtual void LoadModel(const std::string& path, std::vector<D3D::Vertex>& vertices, std::vector<uint32_t>& indices) override;


	};
}

#endif // !D3DModelLoaderIncluded