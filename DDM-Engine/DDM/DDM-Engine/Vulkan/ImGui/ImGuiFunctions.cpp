// ImGuiFunctions.cpp

// Header include
#include "ImGuiFunctions.h"

namespace DDM::UI
{
    bool TreeNode(const std::string& label, int flags)
    {
        return ImGui::TreeNodeEx(label.c_str(), static_cast<ImGuiTreeNodeFlags>(flags));
    }

    void PopTree()
    {
        ImGui::TreePop();
    }

    void Text(const std::string& text)
    {
        ImGui::Text("%s", text.c_str());
    }

    bool SliderFloat(const std::string& label, float* value, float min, float max)
    {
        return ImGui::SliderFloat(label.c_str(), value, min, max);
    }

    DDM_API bool Toggle(const std::string& label, bool* value)
    {
		return ImGui::Checkbox(label.c_str(), value);
    }
}