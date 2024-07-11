// ObjLoader.h
// This class will handle the loading of obj files

#ifndef ObjLoaderIncluded
#define ObjLoaderIncluded

// File includes
#include "../../../Includes/TinyObjLoaderIncludes.h"
#include "../../../DataTypes/Structs.h"

namespace D3D
{
	class ObjLoader
	{
	public:
		ObjLoader() = default;

		~ObjLoader() = default;

		// Load in a obj file given a file path
		// Parameters:
		//     - path: The path to the model file
		//     - vertices: The vector that will be used to store the vertices
		//     - indices: The vector that will be used to store the indices
		void LoadObjModel(const std::string& path, std::vector<D3D::Vertex>& vertices, std::vector<uint32_t>& indices);

	private:

		void SetupTangents(std::vector<D3D::Vertex>& vertices, std::vector<uint32_t>& indices);

	};
}
#endif // ObjLoaderIncluded