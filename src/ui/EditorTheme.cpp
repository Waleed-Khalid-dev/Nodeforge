#include "EditorTheme.h"

namespace nf::ui {

void EditorTheme::ApplyTheme() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // Window & Padding
    style.WindowRounding    = 6.0f;
    style.ChildRounding     = 4.0f;
    style.FrameRounding     = 4.0f;
    style.PopupRounding     = 6.0f;
    style.ScrollbarRounding = 4.0f;
    style.GrabRounding      = 3.0f;
    style.TabRounding       = 5.0f;
    style.WindowBorderSize  = 1.0f;
    style.FrameBorderSize   = 1.0f;
    style.PopupBorderSize   = 1.0f;
    style.ItemSpacing       = ImVec2(8.0f, 6.0f);
    style.ItemInnerSpacing  = ImVec2(6.0f, 4.0f);
    style.FramePadding      = ImVec2(6.0f, 4.0f);
    style.WindowPadding     = ImVec2(10.0f, 10.0f);

    // Neo Realms Charcoal/Slate Color Scheme
    colors[ImGuiCol_Text]                  = ImVec4(0.92f, 0.94f, 0.96f, 1.00f);
    colors[ImGuiCol_TextDisabled]          = ImVec4(0.50f, 0.54f, 0.60f, 1.00f);
    colors[ImGuiCol_WindowBg]              = ImVec4(0.08f, 0.09f, 0.11f, 1.00f);
    colors[ImGuiCol_ChildBg]               = ImVec4(0.11f, 0.12f, 0.14f, 1.00f);
    colors[ImGuiCol_PopupBg]               = ImVec4(0.12f, 0.13f, 0.16f, 0.98f);
    colors[ImGuiCol_Border]                = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
    colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]               = ImVec4(0.14f, 0.15f, 0.18f, 1.00f);
    colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.19f, 0.21f, 0.26f, 1.00f);
    colors[ImGuiCol_FrameBgActive]         = ImVec4(0.23f, 0.26f, 0.32f, 1.00f);
    colors[ImGuiCol_TitleBg]               = ImVec4(0.09f, 0.10f, 0.12f, 1.00f);
    colors[ImGuiCol_TitleBgActive]         = ImVec4(0.13f, 0.14f, 0.18f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.08f, 0.09f, 0.11f, 1.00f);
    colors[ImGuiCol_MenuBarBg]             = ImVec4(0.10f, 0.11f, 0.13f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.09f, 0.10f, 0.12f, 0.50f);
    colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.25f, 0.28f, 0.35f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.32f, 0.36f, 0.45f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.40f, 0.45f, 0.56f, 1.00f);
    colors[ImGuiCol_CheckMark]             = ImVec4(0.35f, 0.65f, 0.95f, 1.00f);
    colors[ImGuiCol_SliderGrab]            = ImVec4(0.35f, 0.65f, 0.95f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.45f, 0.75f, 1.00f, 1.00f);
    colors[ImGuiCol_Button]                = ImVec4(0.16f, 0.18f, 0.22f, 1.00f);
    colors[ImGuiCol_ButtonHovered]         = ImVec4(0.24f, 0.27f, 0.34f, 1.00f);
    colors[ImGuiCol_ButtonActive]          = ImVec4(0.30f, 0.35f, 0.44f, 1.00f);
    colors[ImGuiCol_Header]                = ImVec4(0.18f, 0.20f, 0.25f, 1.00f);
    colors[ImGuiCol_HeaderHovered]         = ImVec4(0.25f, 0.29f, 0.37f, 1.00f);
    colors[ImGuiCol_HeaderActive]          = ImVec4(0.30f, 0.36f, 0.46f, 1.00f);
    colors[ImGuiCol_Separator]             = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
    colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.30f, 0.35f, 0.45f, 1.00f);
    colors[ImGuiCol_SeparatorActive]       = ImVec4(0.40f, 0.50f, 0.65f, 1.00f);
    colors[ImGuiCol_ResizeGrip]            = ImVec4(0.20f, 0.22f, 0.27f, 0.50f);
    colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.35f, 0.40f, 0.50f, 0.75f);
    colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.45f, 0.55f, 0.70f, 1.00f);
    colors[ImGuiCol_Tab]                   = ImVec4(0.12f, 0.13f, 0.16f, 1.00f);
    colors[ImGuiCol_TabHovered]            = ImVec4(0.22f, 0.25f, 0.32f, 1.00f);
    colors[ImGuiCol_TabActive]             = ImVec4(0.17f, 0.19f, 0.24f, 1.00f);
    colors[ImGuiCol_TabUnfocused]          = ImVec4(0.10f, 0.11f, 0.13f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.14f, 0.15f, 0.19f, 1.00f);
}

ImVec4 EditorTheme::GetFamilyColor(NodeFamily family) {
    switch (family) {
        case NodeFamily::TexOp:  return ImVec4(0.95f, 0.60f, 0.15f, 1.0f); // Amber / Orange
        case NodeFamily::ChanOp: return ImVec4(0.20f, 0.70f, 0.95f, 1.0f); // Sky Cyan
        case NodeFamily::GeomOp: return ImVec4(0.20f, 0.85f, 0.50f, 1.0f); // Emerald
        case NodeFamily::MatOp:  return ImVec4(0.95f, 0.25f, 0.40f, 1.0f); // Crimson
        case NodeFamily::DataOp: return ImVec4(0.60f, 0.65f, 0.75f, 1.0f); // Slate Gray
        case NodeFamily::Comp:   return ImVec4(0.40f, 0.50f, 0.95f, 1.0f); // Indigo Blue
        default:                 return ImVec4(0.70f, 0.70f, 0.70f, 1.0f);
    }
}

ImU32 EditorTheme::GetFamilyColorU32(NodeFamily family) {
    ImVec4 col = GetFamilyColor(family);
    return ImColor(col);
}

ImVec4 EditorTheme::GetPinColor(PinType type) {
    switch (type) {
        case PinType::Tex:   return ImVec4(0.95f, 0.60f, 0.15f, 1.0f); // Amber
        case PinType::Chan:  return ImVec4(0.20f, 0.70f, 0.95f, 1.0f); // Cyan
        case PinType::Geom:  return ImVec4(0.20f, 0.85f, 0.50f, 1.0f); // Emerald
        case PinType::Mat:   return ImVec4(0.95f, 0.25f, 0.40f, 1.0f); // Crimson
        case PinType::Data:  return ImVec4(0.60f, 0.65f, 0.75f, 1.0f); // Slate
        case PinType::Any:
        default:             return ImVec4(0.80f, 0.80f, 0.80f, 1.0f);
    }
}

ImU32 EditorTheme::GetPinColorU32(PinType type) {
    ImVec4 col = GetPinColor(type);
    return ImColor(col);
}

} // namespace nf::ui
