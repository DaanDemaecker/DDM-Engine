#pragma once

#include "../BaseClasses/Component.h"

namespace DDM3
{

	class RotatorComponent final : public Component
	{
	public:
		RotatorComponent() = default;
		virtual void Update() override;
	};

}