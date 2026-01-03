// AudioTester.cpp
#include "AudioTester.h"

// Header includes
#include "Includes/ImGuiIncludes.h"
#include "ServiceLocator/ServiceLocator.h"

DDM::AudioTester::AudioTester()
{
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
		ImGui::Checkbox("Toggle muted", &tempIsMuted);

		ImGui::TreePop();
	}

	if (isMutedOriginal != tempIsMuted)
	{
		soundSystem.SetMute(tempIsMuted);
	}

}