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
	m_pAudioSource = GetOwner()->GetComponent<AudioSource>();

	if (m_pAudioSource != nullptr)
	{
		m_pAudioSource->SetShowImGui(true);
	}
}

void DDM::AudioTester::OnGUI()
{
	auto& soundSystem{ ServiceLocator::GetSoundSystem() };


	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Framed;

	// Start tree
	if (ImGui::TreeNodeEx("AudioTest", flags))
	{
		m_MasterVolume = soundSystem.GetMasterVolume();

		if (ImGui::SliderFloat("Volume", &m_MasterVolume, 0, 1))
		{
			ServiceLocator::GetSoundSystem().SetMasterVolume(m_MasterVolume);
		}

		m_IsMuted = soundSystem.IsMuted();

		if (ImGui::Checkbox("Is muted", &m_IsMuted))
		{
			soundSystem.SetMute(m_IsMuted);
		}

		ImGui::InputText("Path to clip to play", m_ClipPath.data(), m_TextLength);

		if(ImGui::Button("Set clip"))
		{
			SetCLip(m_ClipPath.data(), m_TextLength);
		}

		ImGui::TreePop();
	}
}

void DDM::AudioTester::SetCLip(char* filePath, int bufferLength)
{
	if (bufferLength <= 0)
	{
		std::cout << "AudioTester: buffer was of insufficient size\n";
		return;
	}

	if (m_pAudioSource != nullptr)
	{
		m_pAudioSource->SetClip(filePath);
	}
}
