#pragma once

#include <imgui.h>
#include "../graph/NodeTypeInfo.h"
#include "../graph/Pin.h"

namespace nf::ui {

class EditorTheme {
public:
    static void ApplyTheme();
    static ImVec4 GetFamilyColor(NodeFamily family);
    static ImU32 GetFamilyColorU32(NodeFamily family);
    static ImVec4 GetPinColor(PinType type);
    static ImU32 GetPinColorU32(PinType type);

    // Color Palette Constants
    static constexpr ImU32 ColBgDark         = IM_COL32(18, 20, 24, 255);
    static constexpr ImU32 ColBgPanel        = IM_COL32(28, 30, 36, 255);
    static constexpr ImU32 ColBgPanelHover   = IM_COL32(36, 38, 46, 255);
    static constexpr ImU32 ColBorder         = IM_COL32(48, 52, 64, 255);
    static constexpr ImU32 ColBorderActive   = IM_COL32(90, 140, 230, 255);
    static constexpr ImU32 ColGridMinor      = IM_COL32(34, 37, 44, 255);
    static constexpr ImU32 ColGridMajor      = IM_COL32(44, 48, 58, 255);
    static constexpr ImU32 ColNodeBg         = IM_COL32(24, 26, 32, 240);
    static constexpr ImU32 ColNodeBgSelected = IM_COL32(32, 36, 46, 255);
};

} // namespace nf::ui
