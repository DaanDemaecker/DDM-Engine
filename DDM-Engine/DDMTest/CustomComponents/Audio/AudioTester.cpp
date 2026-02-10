// AudioTester.cpp
#include "AudioTester.h"

// Header includes
#include "DDM-Engine/Vulkan/ImGui/ImGuiFunctions.h"
#include "DDM-Engine/ServiceLocator/ServiceLocator.h"
#include "DDM-Engine/EngineComponents/Audio/AudioSource.h"

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
	if (DDM::UI::TreeNode("AudioTest", flags))
	{

		float volume = soundSystem.GetMasterVolume();

		if (DDM::UI::SliderFloat("Volume", &volume, 0, 1))
		{
			ServiceLocator::GetSoundSystem().SetMasterVolume(volume);
		}

		float frequency = soundSystem.GetMasterFrequency();

		if (DDM::UI::SliderFloat("Frequency", &frequency, -3, 3))
		{
			ServiceLocator::GetSoundSystem().SetMasterFrequency(frequency);
		}

		float pitch = soundSystem.GetMasterPitch();

		if (DDM::UI::SliderFloat("Pitch", &pitch, 0, 2))
		{
			ServiceLocator::GetSoundSystem().SetMasterPitch(pitch);
		}
		
		DDM::UI::PopTree();

		return;

		bool isMuted = soundSystem.IsMuted();

		if (ImGui::Checkbox("Muted", &isMuted))
		{
			soundSystem.SetMasterMute(isMuted);
		}

		bool paused = soundSystem.GetMasterPaused();

		if (ImGui::Checkbox("Paused", &paused))
		{
			soundSystem.SetMasterPaused(paused);
		}

		bool is3D = soundSystem.GetMaster3D();

		if (ImGui::Checkbox("3D", &is3D))
		{
			soundSystem.SetMaster3D(is3D);
		}

		if (ImGui::Button("Stop all"))
		{
			soundSystem.StopAll();
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
