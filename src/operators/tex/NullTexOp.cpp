#include "NullTexOp.h"

namespace nf {

NullTexOp::NullTexOp(NodeId id, const std::string& name)
    : TexOp(id, name, "NullTexOp") {
    m_inPin = AddInputPin("input", PinType::Tex);
    m_outPin = AddOutputPin("output", PinType::Tex);

    m_params.AddBool("pass_through", "Pass Through", true);
}

bool NullTexOp::Cook(const CookContext& /*context*/) {
    bool passThrough = GetParam("pass_through").Is<bool>() ? GetParam("pass_through").Get<bool>() : true;
    if (passThrough) {
        SetOutputTexture(GetInputTexture(0));
    } else {
        SetOutputTexture(nullptr);
    }
    return true;
}

} // namespace nf
