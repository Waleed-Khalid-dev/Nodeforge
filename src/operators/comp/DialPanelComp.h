#pragma once

#include "../../graph/Node.h"
#include "../../core/ChannelBuffer.h"

namespace nf {

class DialPanelComp : public Node {
public:
    DialPanelComp(NodeId id, const std::string& name);
    ~DialPanelComp() override = default;

    bool Cook(const CookContext& context) override;

    float GetValue() const;
    float GetAngleDegrees() const;
    void SetValueInteractive(float val);

private:
    Pin* m_inChanPin = nullptr;
    Pin* m_outCompPin = nullptr;
    Pin* m_outChanPin = nullptr;
    ChannelBuffer m_outBuffer;
};

} // namespace nf
