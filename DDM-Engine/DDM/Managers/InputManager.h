#ifndef InputManagerIncluded
#define InputManagerIncluded

// Fileincludes
#include "Engine/Singleton.h"

#include "Includes/GLFWIncludes.h"
#include "Includes/GLMIncludes.h"

// Standard library includes
#include <memory>

namespace DDM
{
	// Class forward declarations
	class Keyboard;
	class Mouse;

	class InputManager final : public Singleton<InputManager>
	{
	public:
		virtual ~InputManager();

		void ProcessInput();

		bool GetKeyPressed(int key);

		bool GetKeyDown(int key);

		bool GetKeyUp(int key);

		bool GetMouseButtonPressed(int button);

		bool GetMouseButtonDown(int button);

		bool GetMouseButtonUp(int button);

		const glm::vec2& GetMousePos();

		const glm::vec2& GetMouseDelta();

		const glm::vec2& GetScrollDelta();
	private:
		friend class Singleton<InputManager>;
		InputManager();

		std::unique_ptr<Keyboard> m_pKeyboard{};

		std::unique_ptr<Mouse> m_pMouse{};
	};
}
#endif // !InputManagerIncluded
