// CubeMapMaterial.h
// This class will serve as the material of the cube map and it will hold all the textures for the cubemap

// Parent include
#include "Material.h"

namespace DDM
{
	// Class forward declaration
	class TextureDescriptorObject;

	enum
	{
		sideRight = 0,
		sideLeft = 1,
		sideUp = 2,
		sideDown = 3,
		sideFront = 4,
		sideBack = 5
	};

	class CubeMapMaterial final : public Material
	{
	public:
		/// <summary>
		/// Default constructor
		/// </summary>
		CubeMapMaterial();

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="pipelineName: ">Name of the pipeline to be used with this material</param>
		CubeMapMaterial(std::string&& pipelineName);

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="pipelineName: ">Name of the pipeline to be used with this material</param>
		CubeMapMaterial(std::string& pipelineName);

		/// <summary>
		/// Default destructor
		/// </summary>
		virtual ~CubeMapMaterial() = default;

		// Delete copy and move functions
		CubeMapMaterial(CubeMapMaterial& other) = delete;
		CubeMapMaterial(CubeMapMaterial&& other) = delete;
		CubeMapMaterial& operator=(CubeMapMaterial& other) = delete;
		CubeMapMaterial& operator=(CubeMapMaterial&& other) = delete;

		// Update the descriptorsets
		// Parameters:
		//     descriptorsets: a vector of the descriptorsets that have to be updated
		//     descriptorObjects: a vector of pointers to descriptorobjects in the same order as the shader code

		/// <summary>
		/// Update the descriptor sets
		/// </summary>
		/// <param name="descriptorSets: ">Vector of the descriptorsets to be updated</param>
		/// <param name="descriptorObjects: ">Vector of pointers to descriptorobjects in the same order as the shader code</param>
		virtual void UpdateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, std::vector<DescriptorObject*>& descriptorObjects) override;


		/// <summary>
		/// Set the right face of the cubemap
		/// </summary>
		/// <param name="filepath: ">File path to the image</param>
		void SetRight(const std::string& filepath);

		/// <summary>
		/// Set the right face of the cubemap
		/// </summary>
		/// <param name="filepath: ">File path to the image</param>
		void SetRight(const std::string&& filepath);

		/// <summary>
		/// Set the left face of the cubemap
		/// </summary>
		/// <param name="filepath: ">File path to the image</param>
		void SetLeft(const std::string& filepath);

		/// <summary>
		/// Set the left face of the cubemap
		/// </summary>
		/// <param name="filepath: ">File path to the image</param>
		void SetLeft(const std::string&& filepath);

		/// <summary>
		/// Set the up face of the cubemap
		/// </summary>
		/// <param name="filepath: ">File path to the image</param>
		void SetUp(const std::string& filepath);

		/// <summary>
		/// Set the up face of the cubemap
		/// </summary>
		/// <param name="filepath: ">File path to the image</param>
		void SetUp(const std::string&& filepath);

		/// <summary>
		/// Set the down face of the cubemap
		/// </summary>
		/// <param name="filepath: ">File path to the image</param>
		void SetDown(const std::string& filepath);

		/// <summary>
		/// Set the down face of the cubemap
		/// </summary>
		/// <param name="filepath: ">File path to the image</param>
		void SetDown(const std::string&& filepath);

		/// <summary>
		/// Set the front face of the cubemap
		/// </summary>
		/// <param name="filepath: ">File path to the image</param>
		void SetFront(const std::string& filepath);

		/// <summary>
		/// Set the front face of the cubemap
		/// </summary>
		/// <param name="filepath: ">File path to the image</param>
		void SetFront(const std::string&& filepath);

		/// <summary>
		/// Set the back face of the cubemap
		/// </summary>
		/// <param name="filepath: ">File path to the image</param>
		void SetBack(const std::string& filepath);

		/// <summary>
		/// Set the back face of the cubemap
		/// </summary>
		/// <param name="filepath: ">File path to the image</param>
		void SetBack(const std::string&& filepath);


	private:
		// Pointer to the descriptor object
		std::unique_ptr<TextureDescriptorObject> m_pDescriptorObject{};

		// The amount of levels the mipmap will have
		uint32_t m_MipLevels{};

		// Vector of filepaths
		std::vector<std::string> m_FilePaths{};

		// Indicates wether texture has been initialized
		bool m_Initialized{};

		/// <summary>
		/// Set the filepath of a single texture in the vector
		/// </summary>
		/// <param name="name: ">Filepath to the image</param>
		/// <param name="index: ">Index of the image in the vector to change</param>
		void SetTextureName(const std::string& name, int index);

		/// <summary>
		/// Load in the cube material
		/// </summary>
		void SetupCubeTexture();
	};
}

