// ModelLoaderComponent.h
// This component will let the user load models trough imgui

#ifndef ModelLoaderComponentIncluded
#define ModelLoaderComponentIncluded

// Parent include
#include "BaseClasses/Component.h"

namespace DDM
{
	class ModelLoader final : public Component
	{
	public:
		/// <summary>
		/// Default constructor
		/// </summary>
		ModelLoader();

		/// <summary>
		/// Destructor
		/// </summary>
		~ModelLoader();

		/// <summary>
		/// OnGui function for imgui rendering
		/// </summary>
		virtual void OnGUI() override;

	private:
		// Max length for text input
		const int m_TextLength{125};

		// Char arrays for text input for object name
		char m_ObjectName[125]{};

		// Char arrays for text input for object filepath
		char m_FilePath[125]{};

		// Boolean indicating if textbox is hovered
		bool m_IsTextboxHovered{false};

		/// <summary>
		/// Callback function for file drop event
		/// </summary>
		/// <param name="count: ">Amount of file paths</param>
		/// <param name="paths: ">Pointer to file paths</param>
		void DropFileCallback(int count, const char** paths);
		
		/// <summary>
		/// Load the object with the given name and filepath
		/// </summary>
		void LoadObject();
	};
}
#endif // !ModelLoaderComponentIncluded