// InfoComponent.h
// This component will display information like the current frame rate and other useful information

#ifndef _INFO_COMPONENT_
#define _INFO_COMPONENT_


// Parent include
#include "BaseClasses/Component.h"

namespace DDM3
{
	class InfoComponent final : public Component
	{
	public:
		InfoComponent();

		virtual void Update() override;

		virtual void OnGUI() override;

	private:
		std::string m_DeltaTimeLabel{ "" };
	
		const int m_FramesPerUpdate{ 5 };
		int m_Frames{};
		int m_DeltaTime{};
	};
}

#endif // !_INFO_COMPONENT_