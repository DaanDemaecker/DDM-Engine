// FmodChannel.h
// This class will serve as a wrapper for the Fmod::Channel class

#ifndef _DDM_FMOD_CHANNEL_
#define _DDM_FMOD_CHANNEL_

// File includes
#include "Includes/FmodIncludes.h"
#include "Events/Subject.h"

namespace DDM
{
	class GameObject;
	struct FmodSystemInfo;
	struct AudioSourceInfo;

	class FmodChannel final : public Subject
	{
	public:
		FmodChannel() = delete;

		FmodChannel(FMOD::Channel* pChannel, int index, const FmodSystemInfo& systemInfo, const AudioSourceInfo& sourceInfo);

		~FmodChannel();

		void SetChannel(FMOD::Channel* pChannel, const FmodSystemInfo& systemInfo, const AudioSourceInfo& sourceInfo);

		void SetMasterMute(bool muted);

		void SetMute(bool muted);

		void SetMasterVolume(float volume);

		void SetVolume(float volume);

		int GetIndex() const { return m_Index; }

		float GetVolume();

		FMOD::Channel* GetChannel() const { return m_pChannel; }

		void UpdateSourceLocation(GameObject* pGameObject);

		void SetMasterPaused(bool paused);

		void SetPaused(bool paused);

		void SetMaster3D(bool is3d);

		void Set3D(bool is3d);

		void SetLoop(bool looping);

		void Stop();

		void SetMasterFrequency(float frequency);

		void SetFrequency(float frequency);

		void SetPriority(int priority);

		int GetPriority() const { return m_Priority; }

	private:
		int m_Priority{ 0 };

		float m_Volume{ 1 };

		float m_MasterVolume{ 1 };

		bool m_Muted{ false };

		bool m_MasterMuted{ false };

		bool m_Paused{ false };

		bool m_MasterPaused{ false };

		bool m_Is3d{ true };

		bool m_Master3d{ true };

		float m_InitialFrequency{};

		float m_Frequency{ 1.f };

		float m_MasterFrequency{ 1.f };

		FMOD::Channel* m_pChannel{};

		bool m_IgnoreNextCallback{false};

		const int m_Index;

		static FMOD_RESULT F_CALLBACK ChannelCallback(
			FMOD_CHANNELCONTROL* channelcontrol,
			FMOD_CHANNELCONTROL_TYPE controltype,
			FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype,
			void* commanddata1,
			void* commanddata2
		);

		void SetVolume();

		void SetMute();

		void SetPaused();

		void Set3D();

		void SetFrequency();

		void SetInfo(const FmodSystemInfo& systemInfo, const AudioSourceInfo& sourceInfo);
	};
}

#endif // !_DDM_FMOD_CHANNEL_
