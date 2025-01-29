#ifndef SpectatorMovementComponentIncluded
#define SpectatorMovementComponentIncluded

#include "../BaseClasses/Component.h"

namespace DDM3
{
	class SpectatorMovementComponent final : public Component
	{
	public:
		SpectatorMovementComponent();
		~SpectatorMovementComponent() = default;

		virtual void Update() override;

	private:
		const float m_Speed{5.f};
		const float m_AngularSpeed{ 0.5f };

		float m_TotalPitch{};
		float m_TotalYaw{};
	};
}


#endif // !SpectatorMovementComponentIncluded