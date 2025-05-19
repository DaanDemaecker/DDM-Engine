// InfoComponent.cpp

// Header include
#include "InfoComponent.h"

//File includes
#include "Includes/ImGuiIncludes.h"
#include "Managers/TimeManager.h"

DDM3::InfoComponent::InfoComponent()
	:Component()
{

}

void DDM3::InfoComponent::Update()
{
	m_Frames++;
	m_DeltaTime += DDM3::TimeManager::GetInstance().GetDeltaTimeMS();

	if (m_Frames >= m_FramesPerUpdate)
	{
		m_DeltaTimeLabel = std::string("Delta time: " + std::to_string(static_cast<int>(m_DeltaTime/m_Frames)) + " ms");
		m_Frames = 0;
		m_DeltaTime = 0;
	}
}

void DDM3::InfoComponent::OnGUI()
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Framed;

	if (ImGui::TreeNodeEx("Info window", flags))
	{
		ImGui::Text(m_DeltaTimeLabel.c_str());
		ImGui::TreePop();
	}
}
