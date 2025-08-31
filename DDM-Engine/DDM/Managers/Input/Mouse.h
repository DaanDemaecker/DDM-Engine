// Mouse.h

#ifndef MouseIncluded
#define MouseIncluded

// File includes
#include "Includes/GLMIncludes.h"

// Standard library includes
#include <memory>

namespace DDM
{
	class MouseImpl;

	class Mouse final
	{
	public:
		Mouse();


		~Mouse();

		Mouse(const Mouse& other) = delete;
		Mouse(Mouse&& other) = delete;

		Mouse& operator=(const Mouse& other) = delete;
		Mouse& operator=(Mouse&& other) = delete;

		void Update();

		bool IsPressed(int button);

		bool IsUp(int button);

		bool IsDown(int button);

		const glm::vec2& GetMousePos();

		const glm::vec2& GetMouseDelta();

		const glm::vec2& GetScrollDelta();

	private:
		std::unique_ptr<MouseImpl> m_pImpl;
	};
}

#endif // !MouseIncluded