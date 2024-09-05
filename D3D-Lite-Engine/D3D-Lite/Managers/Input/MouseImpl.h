// MouseImplImpl.h
// This class will manage the input events from glfw

#ifndef MouseImplIncluded
#define MouseImplIncluded

// Fil includes
#include "Includes/GLMIncludes.h"

// Standard library includes
#include <vector>

namespace D3D
{
	class MouseImpl
	{
	public:

		MouseImpl();
		~MouseImpl() = default;

		MouseImpl(const MouseImpl& other) = delete;
		MouseImpl(MouseImpl&& other) = delete;

		MouseImpl& operator=(const MouseImpl& other) = delete;
		MouseImpl& operator=(MouseImpl&& other) = delete;

		void Update();

		bool IsPressed(int button);

		bool IsUp(int button);

		bool IsDown(int button);

		glm::vec2& GetMousePos() { return m_MousePos; }

		glm::vec2& GetMouseDelta() { return m_MouseDelta; }

	private:
		std::vector<bool> m_Pressed;
		std::vector<bool> m_Up;
		std::vector<bool> m_Down;

		glm::vec2 m_MousePos{};
		glm::vec2 m_MouseDelta{};
	};
}
#endif // !MouseImplIncluded