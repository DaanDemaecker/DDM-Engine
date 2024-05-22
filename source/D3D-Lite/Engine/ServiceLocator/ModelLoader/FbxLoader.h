// FbxLoader.h
// This class will handle the loading of fbx files

#ifndef FbxLoaderIncluded
#define FbxLoaderIncluded

// File includes
#include "../../../Includes/FbxSdkIncludes.h"
#include "../../../DataTypes/Structs.h"

// Standard library includes
#include <vector>
#include <unordered_map>


namespace D3D
{
	class FbxLoader
	{
	public:
		FbxLoader();

		~FbxLoader();


		// Load in a fbx file given a file path
		// Parameters:
		//     - path: The path to the model file
		//     - vertices: The vector that will be used to store the vertices
		//     - indices: The vector that will be used to store the indices
		void LoadFbx(const std::string& path, std::vector<D3D::Vertex>& vertices, std::vector<uint32_t>& indices);

	private:

		FbxManager* m_pFbxManager{};

		struct fbxTexturedInfo
		{
			bool textured{ false };
			int uvIndex{};
			FbxStringList uvSets{};
			int uvSetsCount{};
		};

		struct fbxBoneInfo
		{
			glm::vec4 boneIndices{};
			glm::vec4 boneWeights{};
			int boneAmount{};

			void AddBone(float boneIndex, float boneWeight)
			{
				if (boneAmount >= 4)return;

				boneIndices[boneAmount] = boneIndex;
				boneWeights[boneAmount] = boneWeight;

				boneAmount++;
			}
		};

		struct fbxSkinnedInfo
		{
			bool isSkinned{ false };
			FbxSkin* pSkin{ nullptr };
			std::vector<fbxBoneInfo> boneInfos{};
		};

		void ConvertMesh(FbxMesh* pMesh,
			std::unordered_map<D3D::Vertex, uint32_t>& uniqueVertices, std::vector<D3D::Vertex>& vertices,
			std::vector<uint32_t>& indices, int& baseUvIndex);

		void HandleFbxVertex(FbxMesh* pMesh, FbxVector4* controlPoints, int polygonIndex, int inPolygonPosition,
			std::unordered_map<D3D::Vertex, uint32_t>& uniqueVerticesint,
			fbxTexturedInfo& textureInfo, fbxSkinnedInfo& skinnedInfo,
			std::vector<D3D::Vertex>& vertices, std::vector<uint32_t>& indices);


		void SetupTangents(std::vector<D3D::Vertex>& vertices, std::vector<uint32_t>& indices);

		void SetupSkin(fbxSkinnedInfo& skinnedInfo, int controlPointAmount);
	};
}


#endif // !FbxLoaderIncluded
