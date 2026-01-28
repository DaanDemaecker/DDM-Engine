// AudioSource.cpp

// Header include
#include "AudioSource.h"

// File includes
#include "ServiceLocator/ServiceLocator.h"
#include "Includes/ImGuiIncludes.h"
#include "AudioEvents.h"

// Standard library includes
#include <algorithm>

DDM::AudioSource::AudioSource()
	:Component()
{

}

void DDM::AudioSource::OnGUI()
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Framed;

	// Start tree
	if (ImGui::TreeNodeEx("AudioSource", flags))
	{
		ImGui::Text(m_pClip == nullptr ? "" : m_pClip->GetFilePath().c_str());

		if(ImGui::Button("Play"))
		{
			Play();
		}

		std::string channelText = "Using channel: " + std::to_string(m_Info.Channel);

		ImGui::Text(channelText.c_str());

		if (ImGui::SliderInt("Priority", &m_Info.Priority, 0, 10))
		{
			SetPriority(m_Info.Priority);
		}

		if (ImGui::Checkbox("Muted", &m_Info.Muted))
		{
			SetMute(m_Info.Muted);
		}

		if (ImGui::Checkbox("3D", &m_Info.Is3D))
		{
			Set3D(m_Info.Is3D);
		}

		if(ImGui::Checkbox("Paused", &m_Info.Paused))
		{
			SetPaused(m_Info.Paused);
		}

		if (ImGui::Checkbox("Looping", &m_Info.Looping))
		{
			SetLooping(m_Info.Looping);
		}

		if (ImGui::SliderFloat("Volume", &m_Info.Volume, 0, 1))
		{
			SetVolume(m_Info.Volume);
		}

		if (ImGui::SliderFloat("Frequency", &m_Info.Frequency, -3, 3))
		{
			SetFrequency(m_Info.Frequency);
		}

		if (ImGui::Button("Stop"))
		{
			Stop();
		}

		ImGui::TreePop();
	}
}

void DDM::AudioSource::LateUpdate()
{
	ServiceLocator::GetSoundSystem().UpdateSourceLocation(m_Info, GetOwner());
}

void DDM::AudioSource::SetClip(const std::shared_ptr<AudioClip> clip)
{
	m_pClip = clip;
}

void DDM::AudioSource::SetClip(const std::string& path)
{
	// Create audioclip and forward to overloaded function
	SetClip(std::make_shared<AudioClip>(path));
}

void DDM::AudioSource::SetClip(const std::string&& path)
{
	// Forward to l-value overloaded function
	SetClip(path);
}

std::shared_ptr<DDM::AudioClip> DDM::AudioSource::GetClip() const
{
	return m_pClip;
}

void DDM::AudioSource::Play()
{
	if (m_pClip == nullptr)
	{
		return;
	}

	m_Info.Channel = ServiceLocator::GetSoundSystem().PlayClip(m_pClip.get(), m_Info, this);
}

void DDM::AudioSource::Stop()
{
	ServiceLocator::GetSoundSystem().Stop(m_Info);
}

void DDM::AudioSource::SetLooping(bool looping)
{
	m_Info.Looping = looping;

	ServiceLocator::GetSoundSystem().SetLoop(m_Info);
}

void DDM::AudioSource::SetFrequency(float frequency)
{
	m_Info.Frequency = frequency;

	ServiceLocator::GetSoundSystem().SetFrequency(m_Info);
}

void DDM::AudioSource::SetPriority(int priority)
{
	m_Info.Priority = priority;

	ServiceLocator::GetSoundSystem().SetPriority(m_Info);
}

void DDM::AudioSource::Notify(const Event& event)
{
	if (dynamic_cast<const AudioFinishedEvent*>(&event))
	{
		m_Info.Channel = -1;
	}
}

void DDM::AudioSource::SetVolume(float volume)
{
	m_Info.Volume = std::clamp(volume, 0.0f, 1.0f);

	ServiceLocator::GetSoundSystem().SetVolume(m_Info);
}

void DDM::AudioSource::SetMute(bool mute)
{
	m_Info.Muted = mute;

	ServiceLocator::GetSoundSystem().SetMute(m_Info);
}

void DDM::AudioSource::SetPaused(bool paused)
{
	m_Info.Paused = paused;

	ServiceLocator::GetSoundSystem().SetPaused(m_Info);
}

void DDM::AudioSource::Set3D(bool is3D)
{
	m_Info.Is3D = is3D;

	ServiceLocator::GetSoundSystem().Set3D(m_Info);
}
