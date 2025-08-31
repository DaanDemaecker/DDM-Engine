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

std::queue<glm::vec2> DDM::MouseImpl::m_MouseScroll = std::queue<glm::vec2>();

DDM::MouseImpl::MouseImpl()
{
	m_Pressed.resize(GLFW_MOUSE_BUTTON_LAST + 1);

	m_Down.resize(m_Pressed.size());

	m_Up.resize(m_Pressed.size());

	glfwSetScrollCallback(Window::GetInstance().GetWindowStruct().pWindow, DDM::MouseImpl::MouseScrollCallback);
}

void DDM::MouseImpl::Update()
{
	auto window = DDM::Window::GetInstance().GetWindowStruct().pWindow;

	std::vector<bool> currentState;
	currentState.resize(m_Pressed.size());

	for (int key = GLFW_MOUSE_BUTTON_1; key <= GLFW_MOUSE_BUTTON_LAST; ++key)
	{
		currentState[key] = glfwGetMouseButton(window, key) == GLFW_PRESS;
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

	if (!m_MouseScroll.empty())
	{
		m_MouseScroll.pop();
	}
}

bool DDM::MouseImpl::IsPressed(int button)
{
	return m_Pressed[button];
}

bool DDM::MouseImpl::IsUp(int button)
{
	return m_Up[button];
}

bool DDM::MouseImpl::IsDown(int button)
{
	return m_Down[button];
}

const glm::vec2& DDM::MouseImpl::GetScrollDelta()
{
	if (m_MouseScroll.empty())
		return m_EmptyVec;

	return m_MouseScroll.front();
}

void DDM::MouseImpl::MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	m_MouseScroll.push(glm::vec2(xoffset, yoffset));
}