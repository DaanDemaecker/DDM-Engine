// Rotator.h
// This componenat will rotate the game object it is attached to

#ifndef _ROTATOR_
#define _ROTATOR_

// Parent include
#include "BaseClasses/Component.h"

// File includes
#include "Includes/GLMIncludes.h"

namespace DDM
{
	class RotatorComponent final : public Component
	{
	public:
		/// <summary>
		/// Default constructor
		/// </summary>
		RotatorComponent() = default;

		/// <summary>
		/// Update function
		/// </summary>
		virtual void Update() override;

		/// <summary>
		/// Set the rotation speed in degrees per second
		/// </summary>
		/// <param name="newSpeed: ">New rotation speed</param>
		void SetRotSpeed(float newSpeed) { m_RotationSpeed = newSpeed; }

		/// <summary>
		/// Set rotation axis
		/// </summary>
		/// <param name="axis: ">new axis</param>
		void SetRotAxis(glm::vec3& axis);

		/// <summary>
		/// Set rotation axis
		/// </summary>
		/// <param name="axis: ">new axis</param>
		void SetRotAxis(glm::vec3&& axis);

	private:
		// Rotation speed in degrees per second
		float m_RotationSpeed{ 50.0f };

		// Rotation axis
		glm::vec3 m_RotationAxis{ 0.0f, 1.0f, 0.0f };
	};

}

#endif // _ROTATOR_