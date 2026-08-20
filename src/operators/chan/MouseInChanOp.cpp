#include "MouseInChanOp.h"
#include "io/input/InputManager.h"
#include <algorithm>

namespace nf {

static const std::vector<std::string> s_mouseChannelNames = {
    "x", "y", "dx", "dy", "left", "right", "middle", "wheel_x", "wheel_y", "hover"
};

MouseInChanOp::MouseInChanOp(NodeId id, const std::string& name)
    : ChanOp(id, name, "MouseInChanOp") {
    m_outPin = AddOutputPin("output", PinType::Chan);

    SetParam("scope", static_cast<int32_t>(0)); // 0=Focused, 1=Projector, 2=Global
    SetParam("normalized", true);
    SetParam("active", true);
}

MouseInChanOp::~MouseInChanOp() = default;

bool MouseInChanOp::Cook(const CookContext& /*context*/) {
    bool active = true;
    if (GetParam("active").Is<bool>()) {
        active = GetParam("active").Get<bool>();
    }

    int32_t scope = 0;
    if (GetParam("scope").Is<int32_t>()) {
        scope = std::clamp(GetParam("scope").Get<int32_t>(), 0, 2);
    }

    bool normalized = true;
    if (GetParam("normalized").Is<bool>()) {
        normalized = GetParam("normalized").Get<bool>();
    }

    auto& input = InputManager::Instance();
    MouseState ms = input.GetMouseState(scope);

    ChannelBuffer buf(s_mouseChannelNames, 1, 60.0f);

    if (active) {
        float posX = normalized ? ms.x : ms.rawPixelX;
        float posY = normalized ? ms.y : ms.rawPixelY;

        float vals[10] = {
            posX,
            posY,
            ms.dx,
            ms.dy,
            ms.leftButton ? 1.0f : 0.0f,
            ms.rightButton ? 1.0f : 0.0f,
            ms.middleButton ? 1.0f : 0.0f,
            ms.wheelX,
            ms.wheelY,
            ms.hover ? 1.0f : 0.0f
        };

        for (size_t i = 0; i < 10; ++i) {
            float* p = buf.GetChannelData(i);
            if (p) *p = vals[i];
        }
    }

    SetOutputBuffer(buf);
    return true;
}

} // namespace nf
