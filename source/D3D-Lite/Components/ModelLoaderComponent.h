// ModelLoaderComponent.h
// This component will let the user load models trough imgui

#ifndef ModelLoaderComponentIncluded
#define ModelLoaderComponentIncluded

// File includes
#include "../BaseClasses/Component.h"

// Standard library includes

namespace D3D
{
	class ModelLoaderComponent final : public Component
	{
	public:
		ModelLoaderComponent();
		~ModelLoaderComponent() = default;

		virtual void OnGUI() override;

	private:
		const int m_TextLength{125};
		char m_ObjectName[125]{};
		char m_FilePath[125]{};
		bool m_IsTextboxHovered{false};

		void DropFileCallback(int count, const char** paths);

		void LoadObject();

	};
}
#endif // !ModelLoaderComponentIncluded