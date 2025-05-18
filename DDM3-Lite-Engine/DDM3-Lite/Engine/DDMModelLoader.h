// DDMModelLoader.h
// This class will hold the model loader that can be used to load in 3D models of different types

#ifndef _DDM_MODEL_LOADER_
#define _DDM_MODEL_LOADER_

// Parent class include
#include "Singleton.h"

// File includes
#include "DataTypes/Structs.h"
#include "DDMModelLoader/DDMModelLoader.h"

// Standard library includes
#include <memory>
#include <vector>
#include <string>

namespace DDM3
{
	class GameObject;

	class DDMModelLoader final : public Singleton<DDMModelLoader>
	{
	public:
		DDMModelLoader();

		// Uses TinyObjLoader to store a .obj file in a vertex- and indexVector
		// Parameters:
		//     - filename: The name of the obj file
		//     - vertices: The vector that will be used to store the vertices
		//     - indices: The vector that will be used to store the indices
		void LoadModel(const std::string& filename, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);

		/// <summary>
		/// This function will load in a single model
		/// </summary>
		/// <param name="Path: ">Path to the requested model file</param>
		/// <param name="pParent: ">Parent of the model to be loaded in</param>
		/// <returns>Game object holding rendercomponent</returns>
		GameObject* LoadModel(const std::string& filename, GameObject* pParent);

		/// <summary>
		/// This function will load in a single model into a DDMML Mesh
		/// </summary>
		/// <param name="fileName: ">Path to the requested model file</param>
		/// <param name="mesh: ">reference of mesh to store model in</param>
		void LoadModel(const std::string& fileName, std::unique_ptr<DDMML::Mesh>& mesh);

		/// <summary>
		/// This function will load in a complete scene and set up the textures
		/// </summary>
		/// <param name="path: ">Path to the requested scene file</param>
		/// <param name="pParent: ">Parent of the scene to be loaded in</param>
		void LoadTexturedScene(const std::string& path, GameObject* pParent);


		/// <summary>
		/// Load a scene into a vector of DDMML meshes
		/// </summary>
		/// <param name="path: ">Path to the requested scene file</param>
		/// <param name="pMeshes: ">Vector to store meshes</param>
		void LoadScene(const std::string& path, std::vector<std::unique_ptr<DDMML::Mesh>>& pMeshes);


		/// <summary>
		/// This function converts the vertices from the DDMML::Vertex to the Vertex struct
		/// </summary>
		/// <param name="ddmmlVertices"></param>
		/// <param name="vertices"></param>
		void ConvertVertices(const std::vector<DDMML::Vertex>& ddmmlVertices, std::vector<Vertex>& vertices);

	private:
		std::unique_ptr<DDMML::DDMModelLoader> m_pModelLoader;

		/// <summary>
		/// Converts a single DDMML mesh to a render object
		/// </summary>
		/// <param name="pDDMMLMesh: ">The DDMML mesh</param>
		/// <param name="pParent: ">The parent of the to e created object</param>
		/// <returns>Game object holding rendercomponent</returns>
		GameObject* SetupModel(DDMML::Mesh* pDDMMLMesh, GameObject* pParent);
	};
}


#endif // !_DDM_MODEL_LOADER_