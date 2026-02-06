// ServiceLocator.h
// This class will serve as a locator for services like the soundsystem

#ifndef _DDM_SERVICE_LOCATOR_
#define _DDM_SERVICE_LOCATOR_

// File includes
#include "SoundSystem/SoundSystem.h"

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

		static SoundSystem& GetSoundSystem();
		static void RegisterSoundSystem(std::unique_ptr<SoundSystem> soundSystem);

	private:
		static std::unique_ptr<SoundSystem> m_pSoundSystemInstance;
		static std::unique_ptr<DefaultSoundSystem> m_DefaultSoundSystemInstance;
	};
}

#endif // !_DDM_SERVICE_LOCATOR_
