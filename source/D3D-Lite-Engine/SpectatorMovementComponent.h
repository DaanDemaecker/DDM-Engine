#ifndef SpectatorMovementComponentIncluded
#define SpectatorMovementComponentIncluded

#include "Component.h"

namespace D3D
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

		double m_PrevXPos{};
		double m_PrevYPos{};
	};
}


#endif // !SpectatorMovementComponentIncluded