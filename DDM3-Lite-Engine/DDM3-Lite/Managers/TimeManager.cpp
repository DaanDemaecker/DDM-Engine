// TimeManager.cpp

// Header include
#include "TimeManager.h"

void DDM::TimeManager::SetDeltaTime(float dt)
{
    // Set delta time
    m_DeltaTime = dt;

    // Calculate the amount of frames per second
    m_Fps = static_cast<int>(1.0f / dt);
}