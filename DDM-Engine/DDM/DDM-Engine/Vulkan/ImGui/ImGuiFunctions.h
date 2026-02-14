// ImGuiFunctions.h
// This file will forward the functions of ImGui to the dll

// File includes
#include "DDM-Engine/Includes/ImGuiIncludes.h"
#include "DDM-Engine/Export.h"

// Standard library includes
#include <string>

namespace DDM::UI
{
    DDM_API bool TreeNode(const std::string& label, int flags);
    DDM_API void PopTree();
    DDM_API void Text(const std::string& text);
	DDM_API bool SliderFloat(const std::string& label, float* value, float min, float max);
	DDM_API bool Toggle(const std::string& label, bool* value);
	DDM_API bool Button(const std::string& label, float width = 0, float height = 0);
	DDM_API bool InputText(const std::string& label, char* buffer, size_t bufferSize);
	DDM_API bool Listbox(const std::string& label, int* currentItem, const char* const* items, size_t itemAmount);
}