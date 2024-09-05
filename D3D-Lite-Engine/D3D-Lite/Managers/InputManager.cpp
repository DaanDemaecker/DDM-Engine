// InputManager.cpp

// Header include
#include "InputManager.h"

// File includes
#include "Includes/GLFWIncludes.h"

#include "Engine/Window.h"

#include "Input/Keyboard.h"
#include "Input/Mouse.h"

D3D::InputManager::InputManager()
{
    m_pKeyboard = std::make_unique<Keyboard>();

    m_pMouse = std::make_unique<Mouse>();
}

D3D::InputManager::~InputManager()
{
}

void D3D::InputManager::ProcessInput()
{
    m_pKeyboard->Update();
    m_pMouse->Update();
}

bool D3D::InputManager::GetKeyPressed(int key)
{
    return m_pKeyboard->IsPressed(key);
}

bool D3D::InputManager::GetKeyDown(int key)
{
    return m_pKeyboard->IsDown(key);
}

bool D3D::InputManager::GetKeyUp(int key)
{
    return m_pKeyboard->IsUp(key);
}

bool D3D::InputManager::GetMouseButtonPressed(int button)
{
    return m_pMouse->IsPressed(button);
}

bool D3D::InputManager::GetMouseButtonDown(int button)
{
    return m_pMouse->IsDown(button);
}

bool D3D::InputManager::GetMouseButtonUp(int button)
{
    return m_pMouse->IsUp(button);
}

glm::vec2& D3D::InputManager::GetMousePos()
{
    return m_pMouse->GetMousePos();
}

glm::vec2& D3D::InputManager::GetMouseDelta()
{
    return m_pMouse->GetMouseDelta();
}
