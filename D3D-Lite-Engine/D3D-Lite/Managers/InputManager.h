#ifndef InputManagerIncluded
#define InputManagerIncluded

// Fileincludes
#include "Engine/Singleton.h"

#include "Includes/GLFWIncludes.h"

// Standard library includes
#include <memory>

namespace D3D
{
	// Class forward declarations
	class Keyboard;

	class InputManager final : public Singleton<InputManager>
	{
	public:
		virtual ~InputManager();

		void ProcessInput();

		bool GetKeyPressed(int key);

		bool GetKeyDown(int key);

		bool GetKeyUp(int key);
	private:
		friend class Singleton<InputManager>;
		InputManager();

		std::unique_ptr<Keyboard> m_pKeyboard{};
	};
}
#endif // !InputManagerIncluded
