#include "AnimatorComponent.h"
#include "../Engine/ServiceLocator/ServiceLocator.h"

DDM3::AnimatorComponent::AnimatorComponent()
	:Component()
{
}

void DDM3::AnimatorComponent::AddAnimations(const std::string& path)
{
	ServiceLocator::GetModelLoader().LoadAnimations(path, m_Clips);
}
