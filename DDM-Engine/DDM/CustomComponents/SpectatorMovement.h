// SpectatorMovement.h
// This component will allow the user to move the camera around by using the keyboard and mouse

#ifndef _DDM_SPECTATOR_MOVEMENT_
#define _DDM_SPECTATOR_MOVEMENT_

// Parent include
#include "BaseClasses/Component.h"

namespace DDM
{
	class SpectatorMovement final : public Component
	{
	public:
		/// <summary>
		/// Constructor
		/// </summary>
		SpectatorMovement();

		/// <summary>
		/// Default destructor
		/// </summary>
		~SpectatorMovement() = default;

		/// <summary>
		/// Update function
		/// </summary>
		virtual void Update() override;

	private:
		// Pointer to the transform component of the owning game object
		Transform* m_pTransform{ nullptr };

		// Speed increase
		float m_IncreaseFactor{ 1.1f };

		// Movement speed per second
		float m_Speed{5.f};

		// Angular speed per pixel
		const float m_AngularSpeed{ 0.01f };

		// Total pitch
		float m_TotalPitch{};

		// Total yaw
		float m_TotalYaw{};
	};
}


#endif // !_DDM_SPECTATOR_MOVEMENT_