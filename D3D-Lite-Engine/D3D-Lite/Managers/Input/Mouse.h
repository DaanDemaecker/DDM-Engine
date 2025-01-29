// Mouse.h

#ifndef MouseIncluded
#define MouseIncluded

// File includes
#include "MouseImpl.h"

namespace DDM3
{
	class Mouse final
	{
	public:
		Mouse()
		{
			m_pImpl = std::make_unique<MouseImpl>();
		}


		~Mouse() = default;

		Mouse(const Mouse& other) = delete;
		Mouse(Mouse&& other) = delete;

		Mouse& operator=(const Mouse& other) = delete;
		Mouse& operator=(Mouse&& other) = delete;

		void Update()
		{
			m_pImpl->Update();
		}

		bool IsPressed(int button)
		{
			return m_pImpl->IsPressed(button);
		}

		bool IsUp(int button)
		{
			return m_pImpl->IsUp(button);
		}

		bool IsDown(int button)
		{
			return m_pImpl->IsDown(button);
		}

		glm::vec2& GetMousePos(){ return m_pImpl->GetMousePos(); }

		glm::vec2& GetMouseDelta() { return m_pImpl->GetMouseDelta(); }

	private:
		std::unique_ptr<MouseImpl> m_pImpl;
	};
}

#endif // !MouseIncluded