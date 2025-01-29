// KeyboardImpl.cpp

// Header include
#include "KeyBoardImpl.h"

// File includes
#include "Includes/GLFWIncludes.h"

#include "Engine/Window.h"

// Standard library includes
#include <algorithm>
#include <functional>
#include <iostream>

DDM3::KeyBoardImpl::KeyBoardImpl()
{
	m_Pressed.resize(GLFW_KEY_LAST+1);

	m_Down.resize(m_Pressed.size());

	m_Up.resize(m_Pressed.size());
}

void DDM3::KeyBoardImpl::Update()
{
	auto window = DDM3::Window::GetInstance().GetWindowStruct().pWindow;

	std::vector<bool> currentState;
	currentState.resize(m_Pressed.size());

	for (int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; ++key)
	{
		currentState[key] = glfwGetKey(window, key) == GLFW_PRESS;
	}

	std::vector<bool> changes;
	changes.resize(m_Pressed.size());

	std::transform(currentState.begin(), currentState.end(), m_Pressed.begin(), changes.begin(), std::bit_xor<bool>());

	std::transform(changes.begin(), changes.end(), currentState.begin(), m_Down.begin(), std::bit_and<bool>());

	std::transform(changes.begin(), changes.end(), currentState.begin(), m_Up.begin(), [](bool first, bool second)
		{return std::bit_and<bool>()(first, !second); });

	m_Pressed = std::move(currentState);
}

bool DDM3::KeyBoardImpl::IsPressed(int button)
{
	return m_Pressed[button];
}

bool DDM3::KeyBoardImpl::IsUp(int button)
{
	return m_Up[button];
}

bool DDM3::KeyBoardImpl::IsDown(int button)
{
	return m_Down[button];
}
