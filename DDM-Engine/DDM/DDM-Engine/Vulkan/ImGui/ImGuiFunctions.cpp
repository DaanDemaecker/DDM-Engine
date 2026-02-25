// ImGuiFunctions.cpp

// Header include
#include "ImGuiFunctions.h"

namespace DDM::UI
{
    DDM_API bool TreeNode(const std::string& label, int flags)
    {
        return ImGui::TreeNodeEx(label.c_str(), static_cast<ImGuiTreeNodeFlags>(flags));
    }

    DDM_API void PopTree()
    {
        ImGui::TreePop();
    }

    DDM_API void Text(const std::string& text)
    {
        ImGui::Text("%s", text.c_str());
    }

    DDM_API bool SliderFloat(const std::string& label, float* value, float min, float max)
    {
        return ImGui::SliderFloat(label.c_str(), value, min, max);
    }

    DDM_API bool SliderInt(const std::string& label, int* value, int min, int max)
    {
        return ImGui::SliderInt(label.c_str(), value, min, max);
    }

    DDM_API bool Toggle(const std::string& label, bool* value)
    {
		return ImGui::Checkbox(label.c_str(), value);
    }

    DDM_API bool Button(const std::string& label, float width, float height)
    {
        return ImGui::Button(label.c_str(), ImVec2(width, height));
    }

    DDM_API bool InputText(const std::string& label, char* buffer, size_t bufferSize)
    {
        return ImGui::InputText(label.c_str(), buffer, bufferSize);
    }

    DDM_API bool Listbox(const std::string& label, int* currentItem, const char* const* items, size_t itemAmount)
    {
        return ImGui::ListBox(label.c_str(), currentItem, items, itemAmount);
    }
}