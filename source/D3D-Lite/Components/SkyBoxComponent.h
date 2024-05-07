#ifndef SkyBoxIncluded
#define SkyBoxIncluded

// File includes
#include "MeshRenderComponent.h"
#include "../DataTypes/Mesh.h"
#include "../DataTypes/Materials/Material.h"
#include "../DataTypes/DescriptorObjects/UboDescriptorObject.h"

// Standard library includes
#include <memory>
#include <string>
#include <initializer_list>


namespace D3D
{
	class SkyBoxComponent final : public MeshRenderComponent
	{
	public:
		// Default constructor
		SkyBoxComponent();

		virtual void EarlyUpdate() override;

		// LoadSkybox
		// Parameters:
		//     filePaths: a list of the names to the textures for the faces in the order:
		//                Right, left, up, down, front, back...
		void LoadSkybox(std::initializer_list<const std::string>&& filePaths);

		// Default destructor
		virtual ~SkyBoxComponent() = default;

		// Render the skybox
		void RenderSkyBox();

		void Render() override;
	};

}
#endif // !SkyBoxIncluded