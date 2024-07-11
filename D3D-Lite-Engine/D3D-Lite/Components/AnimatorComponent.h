#ifndef AnimatorComponentIncluded
#define AnimatorComponentIncluded

// File includes
#include "../BaseClasses/Component.h"
#include "../DataTypes/Structs.h"

// Standard library includes
#include <memory>
#include <vector>

namespace D3D
{
	class AnimatorComponent final : public Component
	{
	public:
		AnimatorComponent();
		virtual ~AnimatorComponent() = default;

		void AddAnimations(const std::string& path);

	private:
		std::vector<std::unique_ptr<AnimationClip>> m_Clips{};
	};
}

#endif // !AnimatorComponentIncluded
