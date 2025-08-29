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

		// LoadSkybox
		// Parameters:
		//     filePaths: a list of the names to the textures for the faces in the order:
		//                Right, left, up, down, front, back...

		/// <summary>
		/// Load a new skybox
		/// </summary>
		/// <param name="filePaths: ">List filepaths to textures, faces come in this order: Right, left, up, down,n front, back</param>
		void LoadSkybox(std::initializer_list<const std::string>&& filePaths);

		/// <summary>
		/// Render the skybox
		/// </summary>
		void RenderSkyBox();

		/// <summary>
		/// Renderfunction, overrides the MeshRenderComponent render function
		/// </summary>
		void Render() override;
	};

}
#endif // !SkyBoxIncluded