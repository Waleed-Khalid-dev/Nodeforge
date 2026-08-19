#pragma once

#include "../../graph/Node.h"

namespace nf {

class InOp : public Node {
public:
    InOp(NodeId id, const std::string& name);
    ~InOp() override = default;

    bool Cook(const CookContext& context) override;

    Pin* GetOutputDataPin() const { return m_outputPin; }
    PinType GetPinPayloadType() const;

private:
    Pin* m_outputPin = nullptr;
};

} // namespace nf
