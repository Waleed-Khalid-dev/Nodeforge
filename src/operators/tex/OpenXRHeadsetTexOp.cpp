#include "OpenXRHeadsetTexOp.h"
#include "../../xr/OpenXRRuntime.h"

namespace nf {

OpenXRHeadsetTexOp::OpenXRHeadsetTexOp(NodeId id, const std::string& name)
    : Node(id, name, "OpenXRHeadsetTexOp") {
    m_inLeftEyePin = AddInputPin("in_left_eye", PinType::Tex);
    m_inRightEyePin = AddInputPin("in_right_eye", PinType::Tex);
    m_outPreviewPin = AddOutputPin("output", PinType::Tex);

    SetParam("stereo_mode", static_cast<int32_t>(0)); // 0: Side-by-Side, 1: Dominant Eye
    SetParam("ipd_mm", 64.0f);
    SetParam("mirror_window", true);
    SetParam("enable_passthrough", false);
}

float OpenXRHeadsetTexOp::GetIPD() const {
    return GetParam("ipd_mm").Is<float>() ? GetParam("ipd_mm").Get<float>() : 64.0f;
}

bool OpenXRHeadsetTexOp::IsMirrorWindowEnabled() const {
    return GetParam("mirror_window").Is<bool>() ? GetParam("mirror_window").Get<bool>() : true;
}

bool OpenXRHeadsetTexOp::Cook(const CookContext& /*context*/) {
    // Synchronize IPD with OpenXRRuntime
    float ipdMeters = GetIPD() / 1000.0f;
    OpenXRRuntime::Instance().SetIPD(ipdMeters);

    // Forward left eye texture as companion window preview
    if (m_inLeftEyePin && m_inLeftEyePin->GetValue().Is<std::shared_ptr<gpu::Texture2D>>()) {
        m_outTexture = m_inLeftEyePin->GetValue().Get<std::shared_ptr<gpu::Texture2D>>();
        m_outPreviewPin->SetValue(PinValue(m_outTexture));
    }
    return true;
}

} // namespace nf
