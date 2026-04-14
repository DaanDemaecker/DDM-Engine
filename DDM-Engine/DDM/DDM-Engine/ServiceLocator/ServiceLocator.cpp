// ServiceLocator.cpp

// Header include
#include "ServiceLocator.h"

std::unique_ptr<DDM::DefaultSoundSystem> DDM::ServiceLocator::m_pDefaultSoundSystemInstance{ std::make_unique<DefaultSoundSystem>() };
std::unique_ptr<DDM::SoundSystem> DDM::ServiceLocator::m_pSoundSystemInstance{ nullptr };

std::unique_ptr<DDM::DefaultFileSystem> DDM::ServiceLocator::m_pDefaultFileSystemInstance{ std::make_unique<DefaultFileSystem>() };
std::unique_ptr<DDM::IFileSystem> DDM::ServiceLocator::m_pFileSystemInstance{ nullptr };

void DDM::ServiceLocator::RegisterSoundSystem(std::unique_ptr<DDM::SoundSystem> soundSystem)
{
    if (soundSystem != nullptr)
    {
        m_pSoundSystemInstance = std::move(soundSystem);
    }
}

DDM::SoundSystem& DDM::ServiceLocator::GetSoundSystem()
{
    return m_pSoundSystemInstance.get() == nullptr ? *m_pDefaultSoundSystemInstance.get() : *m_pSoundSystemInstance.get();
}

void DDM::ServiceLocator::RegisterFileSystem(std::unique_ptr<IFileSystem> fileSystem)
{
    if (fileSystem != nullptr)
    {
        m_pFileSystemInstance = std::move(fileSystem);
    }
}

DDM::IFileSystem& DDM::ServiceLocator::GetFileSystem()
{
    return m_pFileSystemInstance.get() == nullptr ? *m_pDefaultFileSystemInstance.get() : *m_pFileSystemInstance.get();
}