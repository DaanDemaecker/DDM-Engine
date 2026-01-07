// AudioSource.cpp

// Header include
#include "AudioSource.h"

// File includes
#include "ServiceLocator/ServiceLocator.h"
#include "Includes/ImGuiIncludes.h"

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
	m_CurrentChannel = ServiceLocator::GetSoundSystem().PlayClip(m_pClip.get());
}
