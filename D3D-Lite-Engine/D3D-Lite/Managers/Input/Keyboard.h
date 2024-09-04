// Keyboard.h

#ifndef KeyboardIncluded
#define KeyboardIncluded

// File includes
#include "KeyBoardImpl.h"

namespace D3D
{
	class Keyboard final
	{
	public:
		Keyboard()
		{
			m_pImpl = std::make_unique<KeyBoardImpl>();
		}


		~Keyboard() = default;

		Keyboard(const Keyboard& other) = delete;
		Keyboard(Keyboard&& other) = delete;

		Keyboard& operator=(const Keyboard& other) = delete;
		Keyboard& operator=(Keyboard&& other) = delete;

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


	private:
		std::unique_ptr<KeyBoardImpl> m_pImpl;
	};
}

#endif // !KeyboardIncluded