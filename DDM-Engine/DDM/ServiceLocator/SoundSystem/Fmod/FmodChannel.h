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

	class FmodChannel final : public Subject
	{
	public:
		FmodChannel() = delete;

		FmodChannel(FMOD::Channel* pChannel, int index);

		~FmodChannel();

		void SetChannel(FMOD::Channel* pChannel);

		void SetMasterMute(bool muted);

		void SetMute(bool muted);

		void SetMasterVolume(float volume);

		void SetVolume(float volume);

		int GetIndex() const { return m_Index; }

		float GetVolume();

		FMOD::Channel* GetChannel() const { return m_pChannel; }

		void UpdateSourceLocation(GameObject* pGameObject);
	private:
		float m_Volume{ 1 };

		float m_MasterVolume{1};

		bool m_Muted{ false };

		bool m_MasterMuted{ false };

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
	};
}

#endif // !_DDM_FMOD_CHANNEL_
