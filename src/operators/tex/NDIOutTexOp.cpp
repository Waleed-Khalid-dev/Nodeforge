#include "NDIOutTexOp.h"

namespace nf {

NDIOutTexOp::NDIOutTexOp(NodeId id, const std::string& name)
    : TexOp(id, name, "NDIOutTexOp") {
    m_inPin = AddInputPin("input", PinType::Tex);
    m_outPin = AddOutputPin("output", PinType::Tex);

    SetParam("stream_name", std::string("NodeForge_NDI"));
    SetParam("active", true);
}

bool NDIOutTexOp::Cook(const CookContext& /*context*/) {
    auto inTex = GetInputTexture(0);
    if (inTex) {
        SetOutputTexture(inTex);
    }
    return true;
}

} // namespace nf
