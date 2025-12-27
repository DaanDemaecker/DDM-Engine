// ServiceLocator.cpp

// Header include
#include "ServiceLocator.h"

std::unique_ptr<DDM::DefaultSoundSystem> DDM::ServiceLocator::m_DefaultSoundSystemInstance{ std::make_unique<DefaultSoundSystem>() };
std::unique_ptr<DDM::SoundSystem> DDM::ServiceLocator::m_pSoundSystemInstance{ nullptr };


void DDM::ServiceLocator::RegisterSoundSystem(std::unique_ptr<DDM::SoundSystem> soundSystem)
{
    if (soundSystem != nullptr)
    {
        m_pSoundSystemInstance = std::move(soundSystem);
    }
}

DDM::SoundSystem& DDM::ServiceLocator::GetSoundSystem()
{
    return m_pSoundSystemInstance.get() == nullptr ? *m_DefaultSoundSystemInstance.get() : *m_pSoundSystemInstance.get();
}
