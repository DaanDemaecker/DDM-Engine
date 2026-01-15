// AudioSource.cpp

// Header include
#include "AudioSource.h"

// File includes
#include "ServiceLocator/ServiceLocator.h"
#include "Includes/ImGuiIncludes.h"
#include "AudioEvents.h"

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

		if (ImGui::Checkbox("Muted", &m_Info.Muted))
		{
			ServiceLocator::GetSoundSystem().SetMute(m_Info);
		}

		if(ImGui::Checkbox("Paused", &m_Info.Paused))
		{
			
		}

		if (ImGui::SliderFloat("Volume", &m_Info.Volume, 0, 1))
		{
			ServiceLocator::GetSoundSystem().SetVolume(m_Info);
		}

		ImGui::TreePop();
	}
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

void DDM::AudioSource::Play()
{
	m_Info.Channel = ServiceLocator::GetSoundSystem().PlayClip(m_pClip.get(), m_Info, this);
}

void DDM::AudioSource::Notify(const Event& event)
{
	if (dynamic_cast<const AudioFinishedEvent*>(&event))
	{
		m_Info.Channel = -1;
	}
}
