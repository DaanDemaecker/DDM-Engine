#include "stdafx.h"
#include "TimeManager.h"

void D3D::TimeManager::UpdateFPS(float deltaTime)
{
    m_ElapsedTime += deltaTime;
    m_FrameCount++;

    if (m_ElapsedTime >= 1.0f)
    {
        m_Fps = m_FrameCount;
        m_FrameCount = 0;
        m_ElapsedTime -= 1.0f;
    }
}

void D3D::TimeManager::SetDeltaTime(float dt)
{
    m_DeltaTime = dt;
    UpdateFPS(dt);
}