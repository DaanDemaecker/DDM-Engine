#pragma once

#include "../BaseClasses/Component.h"

namespace DDM3
{

	class RotatorComponent final : public Component
	{
	public:
		RotatorComponent() = default;
		virtual void Update() override;

		void SetRotSpeed(float newSpeed) { m_RotationSpeed = newSpeed; }

	private:
		float m_RotationSpeed{ 50.0f }; // Rotation speed in degrees per second
	};

}