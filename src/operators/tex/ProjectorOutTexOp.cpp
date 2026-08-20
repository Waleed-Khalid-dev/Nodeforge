#include "ProjectorOutTexOp.h"
#include "../../media/DisplayManager.h"

namespace nf {

ProjectorOutTexOp::ProjectorOutTexOp(NodeId id, const std::string& name)
    : TexOp(id, name, "ProjectorOutTexOp") {
    m_inPin = AddInputPin("input", PinType::Tex);
    m_outPin = AddOutputPin("output", PinType::Tex);

    SetParam("display_index", static_cast<int32_t>(1));
    SetParam("fullscreen", true);
    SetParam("vsync", true);
    SetParam("active", true);
    SetParam("title", std::string("NodeForge Projector Out"));
}

bool ProjectorOutTexOp::Cook(const CookContext& /*context*/) {
    bool active = GetParam("active").Is<bool>() ? GetParam("active").Get<bool>() : true;
    int32_t displayIdx = GetParam("display_index").Is<int32_t>() ? GetParam("display_index").Get<int32_t>() : 1;
    bool fullscreen = GetParam("fullscreen").Is<bool>() ? GetParam("fullscreen").Get<bool>() : true;
    std::string title = GetParam("title").Is<std::string>() ? GetParam("title").Get<std::string>() : "NodeForge Projector Out";

    auto inTex = GetInputTexture(0);
    if (inTex) {
        SetOutputTexture(inTex);
        if (active) {
            OutputWindow* win = DisplayManager::Instance().GetOrCreateWindow(displayIdx, title, fullscreen);
            if (win) {
                win->SetTexture(inTex);
            }
        }
    }

    return true;
}

} // namespace nf
