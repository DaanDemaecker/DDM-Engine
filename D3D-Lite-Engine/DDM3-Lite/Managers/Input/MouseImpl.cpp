// KeyboardImpl.cpp

// Header include
#include "MouseImpl.h"

// File includes
#include "Includes/GLFWIncludes.h"

#include "Engine/Window.h"

// Standard library includes
#include <algorithm>
#include <functional>
#include <iostream>

DDM3::MouseImpl::MouseImpl()
{
	m_Pressed.resize(GLFW_MOUSE_BUTTON_LAST + 1);

	m_Down.resize(m_Pressed.size());

	m_Up.resize(m_Pressed.size());
}

void DDM3::MouseImpl::Update()
{
	auto window = DDM3::Window::GetInstance().GetWindowStruct().pWindow;

	std::vector<bool> currentState;
	currentState.resize(m_Pressed.size());

	for (int key = GLFW_MOUSE_BUTTON_1; key <= GLFW_MOUSE_BUTTON_LAST; ++key)
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

	double x{};
	double y{};

	glfwGetCursorPos(window, &x, &y);

	m_MouseDelta.x = static_cast<float>(x - m_MousePos.x);
	m_MouseDelta.y = static_cast<float>(y - m_MousePos.y);

	m_MousePos.x = static_cast<float>(x);
	m_MousePos.y = static_cast<float>(y);
}

bool DDM3::MouseImpl::IsPressed(int button)
{
	return m_Pressed[button];
}

bool DDM3::MouseImpl::IsUp(int button)
{
	return m_Up[button];
}

bool DDM3::MouseImpl::IsDown(int button)
{
	return m_Down[button];
}
