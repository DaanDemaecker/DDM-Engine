// AudioTester.cpp
#include "AudioTester.h"

// Header includes
#include "Includes/ImGuiIncludes.h"
#include "ServiceLocator/ServiceLocator.h"
#include "EngineComponents/Audio/AudioSource.h"

DDM::AudioTester::AudioTester()
	:Component()
{
	m_ClipPath.resize(m_TextLength);
}

DDM::AudioTester::~AudioTester()
{
}

void DDM::AudioTester::OnSceneLoad()
{
	m_pAudioSource = GetOwner()->AddComponent<AudioSource>();

	m_pAudioSource->SetShowImGui(true);
}

void DDM::AudioTester::OnGUI()
{
	auto& soundSystem{ ServiceLocator::GetSoundSystem() };

	const bool isMutedOriginal{ soundSystem.IsMuted()};
	bool tempIsMuted{ isMutedOriginal };

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Framed;

	// Start tree
	if (ImGui::TreeNodeEx("AudioTest", flags))
	{
		m_Volume = ServiceLocator::GetSoundSystem().GetMasterVolume();

		if (ImGui::SliderFloat("Volume", &m_Volume, 0, 1))
		{
			ServiceLocator::GetSoundSystem().SetMasterVolume(m_Volume);
		}



		ImGui::Checkbox("Is muted", &tempIsMuted);

		ImGui::InputText("Path to clip to play", m_ClipPath.data(), m_TextLength);

		if(ImGui::Button("Set clip"))
		{
			SetCLip(m_ClipPath.data(), m_TextLength);
		}

		ImGui::TreePop();
	}

	if (isMutedOriginal != tempIsMuted)
	{
		soundSystem.SetMute(tempIsMuted);
	}
}

void DDM::AudioTester::SetCLip(char* filePath, int bufferLength)
{
	if (bufferLength <= 0)
	{
		std::cout << "AudioTester: buffer was of insufficient size\n";
		return;
	}

	m_pAudioSource->SetClip(filePath);
}
