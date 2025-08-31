// Mouse.cpp

// Header include
#include "Mouse.h"

// File includes
#include "MouseImpl.h"

DDM::Mouse::Mouse()
{
	m_pImpl = std::make_unique<MouseImpl>();
}

DDM::Mouse::~Mouse()
{
}

void DDM::Mouse::Update()
{
	m_pImpl->Update();
}

bool DDM::Mouse::IsPressed(int button)
{
	return m_pImpl->IsPressed(button);
}

bool DDM::Mouse::IsUp(int button)
{
	return m_pImpl->IsUp(button);
}

bool DDM::Mouse::IsDown(int button)
{
	return m_pImpl->IsDown(button);
}

const glm::vec2& DDM::Mouse::GetMousePos()
{
	return m_pImpl->GetMousePos();
}

const glm::vec2& DDM::Mouse::GetMouseDelta()
{
	return m_pImpl->GetMouseDelta();
}

const glm::vec2& DDM::Mouse::GetScrollDelta()
{
	return m_pImpl->GetScrollDelta();
}
