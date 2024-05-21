// D3DModelLoader.h
// This class is the standard model loader, users can replace it by registering another model loader to the service locator

#ifndef D3DModelLoaderIncluded
#define D3DModelLoaderIncluded

// File includes
#include "ModelLoader.h"
#include "../../../Includes/FbxSdkIncludes.h"

// Standard library includes
#include <memory>

namespace D3D
{
	// Class forward declarations
	class ObjLoader;

	class D3DModelLoader final : public ModelLoader
	{
	public:
		D3DModelLoader();
		virtual ~D3DModelLoader() override;

		// Load in a 3D model given a file path
		// Parameters:
		//     - path: The path to the model file
		//     - vertices: The vector that will be used to store the vertices
		//     - indices: The vector that will be used to store the indices
		virtual void LoadModel(const std::string& path, std::vector<D3D::Vertex>& vertices, std::vector<uint32_t>& indices) override;
	
	private:
		std::unique_ptr<ObjLoader> m_pObjLoader{};


		FbxManager* m_pFbxManager{};

		struct fbxTexturedInfo
		{
			bool textured{ false };
			int uvIndex{};
			FbxStringList uvSets{};
			int uvSetsCount{};
		};

		struct fbxSkinnedInfo
		{
			bool isSkinned{ false };
			FbxSkin* pSkin{nullptr};
		};

		// Load in a fbx file given a file path
		// Parameters:
		//     - path: The path to the model file
		//     - vertices: The vector that will be used to store the vertices
		//     - indices: The vector that will be used to store the indices
		void LoadFbx(const std::string& path, std::vector<D3D::Vertex>& vertices, std::vector<uint32_t>& indices);
	

		void ConvertMesh(FbxMesh* pMesh, std::vector<D3D::Vertex>& vertices, std::vector<uint32_t>& indices, int& baseUvIndex);

		void HandleFbxVertex(FbxMesh* pMesh, FbxVector4* controlPoints, int polygonIndex, int inPolygonPosition,
			std::unordered_map<D3D::Vertex, uint32_t>& uniqueVerticesint, fbxTexturedInfo& textureInfo,
			std::vector<D3D::Vertex>& vertices, std::vector<uint32_t>& indices);
	};
}

#endif // !D3DModelLoaderIncluded