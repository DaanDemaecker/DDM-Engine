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
}