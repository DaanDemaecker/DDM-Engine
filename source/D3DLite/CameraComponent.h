#ifndef CameraComponentIncluded
#define CameraComponentIncluded

#include "Component.h"

namespace D3D
{
	class CameraComponent final : public Component
	{
	public:
		CameraComponent();
		~CameraComponent() = default;

		void SetFovAngle(float angle) { m_FovAngle = angle; }
		void SetFovAngleDegrees(float angle);
		

		float GetFovAngle() const { return m_FovAngle; }

	private:
		float m_FovAngle{};
		const float m_DefaultAngleDegrees{ 45.f };
	};
}
#endif // !CameraComponentIncluded
