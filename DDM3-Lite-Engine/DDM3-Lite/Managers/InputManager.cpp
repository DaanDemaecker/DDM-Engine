// InputManager.cpp

// Header include
#include "InputManager.h"

// File includes
#include "Includes/GLFWIncludes.h"

#include "Engine/Window.h"

#include "Input/Keyboard.h"
#include "Input/Mouse.h"

DDM::InputManager::InputManager()
{
    m_pKeyboard = std::make_unique<Keyboard>();

    m_pMouse = std::make_unique<Mouse>();
}

DDM::InputManager::~InputManager()
{
}

void DDM::InputManager::ProcessInput()
{
    m_pKeyboard->Update();
    m_pMouse->Update();
}

bool DDM::InputManager::GetKeyPressed(int key)
{
    return m_pKeyboard->IsPressed(key);
}

bool DDM::InputManager::GetKeyDown(int key)
{
    return m_pKeyboard->IsDown(key);
}

bool DDM::InputManager::GetKeyUp(int key)
{
    return m_pKeyboard->IsUp(key);
}

bool DDM::InputManager::GetMouseButtonPressed(int button)
{
    return m_pMouse->IsPressed(button);
}

bool DDM::InputManager::GetMouseButtonDown(int button)
{
    return m_pMouse->IsDown(button);
}

bool DDM::InputManager::GetMouseButtonUp(int button)
{
    return m_pMouse->IsUp(button);
}

glm::vec2& DDM::InputManager::GetMousePos()
{
    return m_pMouse->GetMousePos();
}

glm::vec2& DDM::InputManager::GetMouseDelta()
{
    return m_pMouse->GetMouseDelta();
}
