#pragma once

#include "../BaseClasses/Component.h"

namespace D3D
{

	class RotatorComponent final : public Component
	{
	public:
		RotatorComponent() = default;
		virtual void Update() override;
	};

}