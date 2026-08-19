#pragma once

#include "../../graph/Node.h"

namespace nf {

class OutOp : public Node {
public:
    OutOp(NodeId id, const std::string& name);
    ~OutOp() override = default;

    bool Cook(const CookContext& context) override;

    Pin* GetInputDataPin() const { return m_inputPin; }
    PinType GetPinPayloadType() const;

private:
    Pin* m_inputPin = nullptr;
};

} // namespace nf
