// Skybox.h
// This component will render a skybox

#ifndef SkyBoxIncluded
#define SkyBoxIncluded

// Parent include
#include "Components/MeshRenderer.h"

// File includes
#include "DataTypes/Mesh.h"
#include "DataTypes/Materials/Material.h"
#include "DataTypes/DescriptorObjects/UboDescriptorObject.h"

// Standard library includes
#include <memory>
#include <string>
#include <initializer_list>


namespace DDM
{
	class CubeMapMaterial;

	class SkyBoxComponent final : public MeshRenderComponent
	{
	public:
		/// <summary>
		/// Default constructor
		/// </summary>
		SkyBoxComponent();

		/// <summary>
		/// Default destructor
		/// </summary>
		virtual ~SkyBoxComponent() = default;

		/// <summary>
		/// Early update function
		/// </summary>
		virtual void EarlyUpdate() override;

		/// <summary>
		/// Render the skybox
		/// </summary>
		void RenderSkyBox();

		/// <summary>
		/// Renderfunction, overrides the MeshRenderComponent render function
		/// </summary>
		void Render() override;

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
		std::shared_ptr<CubeMapMaterial> m_pCubeMaterial{};

	};

}
#endif // !SkyBoxIncluded