#include "SpoutOutTexOp.h"

namespace nf {

SpoutOutTexOp::SpoutOutTexOp(NodeId id, const std::string& name)
    : TexOp(id, name, "SpoutOutTexOp") {
    m_inPin = AddInputPin("input", PinType::Tex);
    m_outPin = AddOutputPin("output", PinType::Tex);

    SetParam("sender_name", std::string("NodeForge_Output"));
    SetParam("active", true);
}

bool SpoutOutTexOp::Cook(const CookContext& /*context*/) {
    auto inTex = GetInputTexture(0);
    if (inTex) {
        SetOutputTexture(inTex);
    }
    return true;
}

} // namespace nf
