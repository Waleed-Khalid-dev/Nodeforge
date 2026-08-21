#include "PanelComp.h"
#include <algorithm>

namespace nf {

PanelComp::PanelComp(NodeId id, const std::string& name)
    : Node(id, name, "PanelComp") {
    m_inUiPin = AddInputPin("in_ui", PinType::Any);
    m_inBgTexPin = AddInputPin("bg_tex", PinType::Tex);
    m_outCompPin = AddOutputPin("out_ui", PinType::Any);
    m_outChanPin = AddOutputPin("out_chan", PinType::Chan);
    m_outTexPin = AddOutputPin("out_tex", PinType::Tex);

    SetParam("panel_width", static_cast<int32_t>(800));
    SetParam("panel_height", static_cast<int32_t>(600));
    SetParam("layout_mode", static_cast<int32_t>(1)); // 0: Absolute, 1: Flex Row, 2: Flex Column, 3: Grid
    SetParam("spacing", 10.0f);
    SetParam("padding", 12.0f);
    SetParam("bg_color", glm::vec4(0.08f, 0.09f, 0.12f, 0.95f));
    SetParam("border_color", glm::vec4(0.2f, 0.25f, 0.35f, 1.0f));
    SetParam("border_radius", 8.0f);

    m_outBuffer.Resize(1, 1);
    m_outBuffer.SetChannelNames({"active"});
    m_outBuffer.GetChannelData(0)[0] = 1.0f;
}

int32_t PanelComp::GetLayoutMode() const {
    return GetParam("layout_mode").Is<int32_t>() ? GetParam("layout_mode").Get<int32_t>() : 1;
}

glm::ivec2 PanelComp::GetPanelSize() const {
    int32_t w = GetParam("panel_width").Is<int32_t>() ? GetParam("panel_width").Get<int32_t>() : 800;
    int32_t h = GetParam("panel_height").Is<int32_t>() ? GetParam("panel_height").Get<int32_t>() : 600;
    return glm::ivec2(std::max(1, w), std::max(1, h));
}

bool PanelComp::Cook(const CookContext& /*context*/) {
    // If background texture is connected, forward it to out_tex
    if (m_inBgTexPin && m_inBgTexPin->GetValue().Is<std::shared_ptr<gpu::Texture2D>>()) {
        auto inTex = m_inBgTexPin->GetValue().Get<std::shared_ptr<gpu::Texture2D>>();
        if (inTex) {
            m_outTexPin->SetValue(PinValue(inTex));
        }
    }

    // Aggregate status channel
    m_outBuffer.Resize(1, 1);
    m_outBuffer.SetChannelNames({"active"});
    m_outBuffer.GetChannelData(0)[0] = 1.0f;

    m_outChanPin->SetValue(PinValue(m_outBuffer));
    return true;
}

} // namespace nf
