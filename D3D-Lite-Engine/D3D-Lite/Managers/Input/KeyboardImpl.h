// KeyboardImpl.h
// This class will manage the input events from glfw

#ifndef KeyboardImplIncluded
#define KeyboardImplIncluded

// Standard library includes
#include <vector>

namespace D3D
{
	class KeyBoardImpl
	{
	public:

		KeyBoardImpl();
		~KeyBoardImpl() = default;

		KeyBoardImpl(const KeyBoardImpl& other) = delete;
		KeyBoardImpl(KeyBoardImpl&& other) = delete;

		KeyBoardImpl& operator=(const KeyBoardImpl& other) = delete;
		KeyBoardImpl& operator=(KeyBoardImpl&& other) = delete;

		void Update();

		bool IsPressed(int button);

		bool IsUp(int button);

		bool IsDown(int button);

	private:
		std::vector<bool> m_Pressed;
		std::vector<bool> m_Up;
		std::vector<bool> m_Down;
	};
}
#endif // !KeyboardImplIncluded