// AudioTester.cpp
#include "AudioTester.h"

// Header includes
#include "Includes/ImGuiIncludes.h"
#include "ServiceLocator/ServiceLocator.h"

DDM::AudioTester::AudioTester()
{
	m_ClipPath.resize(m_TextLength);
}

DDM::AudioTester::~AudioTester()
{
}

void DDM::AudioTester::OnGUI()
{
	auto& soundSystem{ ServiceLocator::GetSoundSystem() };

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Framed;

	const bool isMutedOriginal{ soundSystem.IsMuted()};
	bool tempIsMuted{ isMutedOriginal };

	// Start tree
	if (ImGui::TreeNodeEx("AudioTest", flags))
	{
		ImGui::Checkbox("Is muted", &tempIsMuted);

		ImGui::InputText("Path to clip to play", m_ClipPath.data(), m_TextLength);

		if(ImGui::Button("Play clip"))
		{
			PlayCLip(m_ClipPath.data(), m_TextLength);
		}

		ImGui::TreePop();
	}

	if (isMutedOriginal != tempIsMuted)
	{
		soundSystem.SetMute(tempIsMuted);
	}
}

void DDM::AudioTester::PlayCLip(char* filePath, int bufferLength)
{
	if (bufferLength <= 0)
	{
		std::cout << "AudioTester: buffer was of insufficient size\n";
		return;
	}

	std::shared_ptr<AudioClip> clip = std::make_shared<AudioClip>(filePath);

	ServiceLocator::GetSoundSystem().PlayClip(clip.get());
}
