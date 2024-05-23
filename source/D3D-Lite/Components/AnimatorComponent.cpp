#include "AnimatorComponent.h"
#include "../Engine/ServiceLocator/ServiceLocator.h"

D3D::AnimatorComponent::AnimatorComponent()
	:Component()
{
}

void D3D::AnimatorComponent::AddAnimations(const std::string& path)
{
	ServiceLocator::GetModelLoader().LoadAnimations(path, m_Clips);
}
