#ifndef InputManagerIncluded
#define InputManagerIncluded

// Fileincludes
#include "../Engine/Singleton.h"

namespace D3D
{
	class InputManager final : public Singleton<InputManager>
	{
	public:
		virtual ~InputManager() = default;

	private:
		friend class Singleton<InputManager>;
		InputManager();
	};
}
#endif // !InputManagerIncluded
