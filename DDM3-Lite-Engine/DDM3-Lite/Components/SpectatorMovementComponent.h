#ifndef SpectatorMovementComponentIncluded
#define SpectatorMovementComponentIncluded

#include "../BaseClasses/Component.h"

namespace DDM
{
	class SpectatorMovementComponent final : public Component
	{
	public:
		SpectatorMovementComponent();
		~SpectatorMovementComponent() = default;

		virtual void Update() override;

	private:
		TransformComponent* m_pTransform{ nullptr };

		const float m_Speed{5.f};
		const float m_AngularSpeed{ 0.01f };

		float m_TotalPitch{};
		float m_TotalYaw{};
	};
}


#endif // !SpectatorMovementComponentIncluded