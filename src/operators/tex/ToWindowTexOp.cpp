#include "ToWindowTexOp.h"

namespace nf {

ToWindowTexOp::ToWindowTexOp(NodeId id, const std::string& name)
    : TexOp(id, name, "ToWindowTexOp") {
    m_inPin = AddInputPin("input", PinType::Tex);
    m_outPin = AddOutputPin("output", PinType::Tex);

    m_params.AddBool("fit", "Fit to Window", true);
    m_params.AddBool("enable", "Enable Output", true);
}

bool ToWindowTexOp::Cook(const CookContext& /*context*/) {
    auto inTex = GetInputTexture(0);
    SetOutputTexture(inTex);
    return true;
}

} // namespace nf
