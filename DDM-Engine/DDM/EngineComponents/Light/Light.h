// Light.h
// This file holds the Light struct, used for sending lighting information to the shader

#ifndef _DDM_LIGHT_
#define _DDM_LIGHT_

// File includes
#include "Includes/GLMIncludes.h"

namespace DDM
{
	// Light types
	enum LightType
	{
		Directional = 0,
		Point = 1,
		Spot = 2
	};

#pragma warning(push)
	// Disable warning C4324
#pragma warning(disable : 4324)
	// Directional light object
	struct Light
	{
		// Type of light
		alignas(4) LightType type{ LightType::Directional };

		// Intensity of light
		alignas(4) float intensity{ 1.0f };

		// Range of light
		alignas(4) float range{ 10 };

		// Angle of light
		alignas(4) float angle{ glm::radians(30.0f) };

		// Direction of light
		alignas(16) glm::vec3 direction{ 1, 0, 0 };

		// Direction of light
		alignas(16) glm::vec3 position{ 1, 0, 0 };

		// Color of light
		alignas(16) glm::vec3 color{ 1, 1, 1 };
	};
#pragma warning(pop)
}

#endif // !_DDM_LIGHT_
