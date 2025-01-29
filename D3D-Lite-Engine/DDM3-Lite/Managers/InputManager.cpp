// InputManager.cpp

// Header include
#include "InputManager.h"

// File includes
#include "Includes/GLFWIncludes.h"

#include "Engine/Window.h"

#include "Input/Keyboard.h"
#include "Input/Mouse.h"

DDM3::InputManager::InputManager()
{
    m_pKeyboard = std::make_unique<Keyboard>();

    m_pMouse = std::make_unique<Mouse>();
}

DDM3::InputManager::~InputManager()
{
}

void DDM3::InputManager::ProcessInput()
{
    m_pKeyboard->Update();
    m_pMouse->Update();
}

bool DDM3::InputManager::GetKeyPressed(int key)
{
    return m_pKeyboard->IsPressed(key);
}

bool DDM3::InputManager::GetKeyDown(int key)
{
    return m_pKeyboard->IsDown(key);
}

bool DDM3::InputManager::GetKeyUp(int key)
{
    return m_pKeyboard->IsUp(key);
}

bool DDM3::InputManager::GetMouseButtonPressed(int button)
{
    return m_pMouse->IsPressed(button);
}

bool DDM3::InputManager::GetMouseButtonDown(int button)
{
    return m_pMouse->IsDown(button);
}

bool DDM3::InputManager::GetMouseButtonUp(int button)
{
    return m_pMouse->IsUp(button);
}

glm::vec2& DDM3::InputManager::GetMousePos()
{
    return m_pMouse->GetMousePos();
}

glm::vec2& DDM3::InputManager::GetMouseDelta()
{
    return m_pMouse->GetMouseDelta();
}
