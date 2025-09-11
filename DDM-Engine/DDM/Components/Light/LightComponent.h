// LightComponent.h
// This class attaches the Light struct to a component

#ifndef _DDM_LIGHT_COMPONENT_
#define _DDM_LIGHT_COMPONENT_

// Parent include
#include "BaseClasses/Component.h"

// File includes
#include "Components/Light/Light.h"
#include "DataTypes/DescriptorObjects/UboDescriptorObject.h"

namespace DDM
{
	class LightComponent final : public Component
	{
	public:
		/// <summary>
		/// Default constructor
		/// </summary>
		LightComponent();

		/// <summary>
		/// Default destructor
		/// </summary>
		~LightComponent() = default;

		// Rule of five
		LightComponent(const LightComponent& other) = delete;
		LightComponent(LightComponent&& other) = delete;

		LightComponent& operator=(const LightComponent& other) = delete;
		LightComponent& operator=(LightComponent&& other) = delete;


		/// <summary>
		/// OnGui function
		/// </summary>
		virtual void OnGUI() override;

		/// <summary>
		/// Late update function
		/// </summary>
		virtual void LateUpdate() override;

		/// <summary>
		/// Set the color of the light
		/// </summary>
		/// <param name="color: ">new color</param>
		void SetColor(glm::vec3& color);

		/// <summary>
		/// Set the color of the light
		/// </summary>
		/// <param name="color: ">new color</param>
		void SetColor(glm::vec3&& color);

		/// <summary>
		/// Set intensity of the light
		/// </summary>
		/// <param name="intensity: ">new intensity</param>
		void SetIntensity(float intensity);

		/// <summary>
		/// Set the type of light
		/// </summary>
		/// <param name="type"></param>
		void SetType(LightType type);

		/// <summary>
		/// Set angle of light
		/// </summary>
		/// <param name="angle: ">new angle in radians</param>
		void SetAngle(float angle);

		/// <summary>
		/// Set angle of light
		/// </summary>
		/// <param name="angle: ">new angle in degrees</param>
		void SetAngleDeg(float angle);

		/// <summary>
		/// Set range of light
		/// </summary>
		/// <param name="range: ">new range</param>
		void SetRange(float range);

		/// <summary>
		/// Gets the descriptor object that holds the light data
		/// </summary>
		/// <returns>Pointer to the descriptor object</returns>
		DescriptorObject* GetDescriptorObject();

	private:
		const char* m_LightTypeStrings[3]{ "Directional", "Point", "Spot" };


		// Struct that holds the values of the light
		Light m_BufferObject{};

		// Pointer to the descriptor object
		std::unique_ptr<UboDescriptorObject<Light>> m_DescriptorObject{};

		// Function for updating the vulkan buffers
		// Parameters:
		//     frame: which frame in flight it currently is
		void UpdateBuffer(int frame);
	};
}



#endif // !_LIGHT_COMPONENT_