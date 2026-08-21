#pragma once

#include "../../graph/Node.h"
#include "../../core/ChannelBuffer.h"

namespace nf {

class ButtonPanelComp : public Node {
public:
    ButtonPanelComp(NodeId id, const std::string& name);
    ~ButtonPanelComp() override = default;

    bool Cook(const CookContext& context) override;

    bool GetState() const;
    void Click();
    void SetPressed(bool pressed);

private:
    Pin* m_inChanPin = nullptr;
    Pin* m_outCompPin = nullptr;
    Pin* m_outChanPin = nullptr;
    ChannelBuffer m_outBuffer;
    bool m_isPressed = false;
};

} // namespace nf
