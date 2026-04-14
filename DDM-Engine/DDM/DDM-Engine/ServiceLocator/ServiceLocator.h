// ServiceLocator.h
// This class will serve as a locator for services like the soundsystem

#ifndef _DDM_SERVICE_LOCATOR_
#define _DDM_SERVICE_LOCATOR_

// File includes
#include "SoundSystem/SoundSystem.h"
#include "FileSystem/IFileSystem.h"

// Export include
#include "DDM-Engine/Export.h"

// File includes
#include <memory>

namespace DDM
{
	class DDM_API ServiceLocator
	{
	public:
		ServiceLocator() = default;
		~ServiceLocator() = default;

		ServiceLocator(ServiceLocator&) = delete;
		ServiceLocator(ServiceLocator&&) = delete;

		ServiceLocator& operator=(ServiceLocator&) = delete;
		ServiceLocator& operator=(ServiceLocator&&) = delete;

		static void RegisterSoundSystem(std::unique_ptr<SoundSystem> soundSystem);
		static SoundSystem& GetSoundSystem();


		static void RegisterFileSystem(std::unique_ptr<IFileSystem> fileSystem);
		static IFileSystem& GetFileSystem();

	private:
		static std::unique_ptr<SoundSystem> m_pSoundSystemInstance;
		static std::unique_ptr<DefaultSoundSystem> m_pDefaultSoundSystemInstance;


		static std::unique_ptr<IFileSystem> m_pFileSystemInstance;
		static std::unique_ptr<DefaultFileSystem> m_pDefaultFileSystemInstance;
	};
}

#endif // !_DDM_SERVICE_LOCATOR_
