// InputManager.cpp

// Header include
#include "InputManager.h"

// File includes
#include "Includes/GLFWIncludes.h"

#include "Engine/Window.h"

#include "Input/Keyboard.h"

D3D::InputManager::InputManager()
{
    m_pKeyboard = std::make_unique<Keyboard>();
}

D3D::InputManager::~InputManager()
{
}

void D3D::InputManager::ProcessInput()
{
    m_pKeyboard->Update();
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
