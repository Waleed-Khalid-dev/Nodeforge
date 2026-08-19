#include "InOp.h"

namespace nf {

InOp::InOp(NodeId id, const std::string& name)
    : Node(id, name, "InOp") {
    m_outputPin = AddOutputPin("output", PinType::Any);

    SetParam("pin_name", std::string("in1"));
    SetParam("pin_type", static_cast<int32_t>(0)); // 0: Any, 1: Tex, 2: Chan, 3: Geom, 4: Mat, 5: Data
}

PinType InOp::GetPinPayloadType() const {
    int32_t typeIdx = 0;
    if (GetParam("pin_type").Is<int32_t>()) {
        typeIdx = GetParam("pin_type").Get<int32_t>();
    }
    switch (typeIdx) {
        case 1: return PinType::Tex;
        case 2: return PinType::Chan;
        case 3: return PinType::Geom;
        case 4: return PinType::Mat;
        case 5: return PinType::Data;
        default: return PinType::Any;
    }
}

bool InOp::Cook(const CookContext& /*context*/) {
    // Value is directly seeded by the parent ContainerComp before inner graph cook
    return true;
}

} // namespace nf
