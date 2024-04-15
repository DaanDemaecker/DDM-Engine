#ifndef DirectionalLightComponentIncluded
#define DirectionalLightComponentIncluded


#include "Component.h"
#include "Structs.h"
#include "UboDescriptorObject.h"

namespace D3D
{
	class DirectionalLightComponent final : public Component
	{
	public:
		DirectionalLightComponent();
		~DirectionalLightComponent() = default;

		virtual void OnGUI() override;

		virtual void LateUpdate() override;

		// Set the color of the light
		// Parameters:
		//     direction: vector for the color
		void SetColor(glm::vec3& color);

		// Set the color of the light with R-value
		// Parameters:
		//     direction: vector for the color
		void SetColor(glm::vec3&& color);

		// Set the intensity of the light
		// Parameters:
		//     intensity: the new intensity of the light
		void SetIntensity(float intensity);

		// Public getter for the vulkan buffers needed for the shader
		DescriptorObject* GetDescriptorObject();

	private:
		// Struct that holds the values of the light
		DirectionalLightStruct m_BufferObject{};

		// Pointer to the descriptor object
		std::unique_ptr<UboDescriptorObject<DirectionalLightStruct>> m_DescriptorObject{};

		// Function for creating the buffers
		void CreateLightBuffer();

		// Function for updating the vulkan buffers
		// Parameters:
		//     frame: which frame in flight it currently is
		void UpdateBuffer(int frame);
	};
}



#endif // !DirectionalLightComponentIncluded