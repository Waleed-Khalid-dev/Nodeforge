#pragma once

#include "../../graph/Node.h"
#include "../../core/ChannelBuffer.h"

namespace nf {

class PhysicsForceChanOp : public Node {
public:
    PhysicsForceChanOp(NodeId id, const std::string& name);
    ~PhysicsForceChanOp() override = default;

    bool Cook(const CookContext& context) override;

    glm::vec3 GetDirection() const;
    glm::vec3 GetOrigin() const;
    float GetStrength() const;
    float GetRadius() const;

private:
    Pin* m_inTriggerPin = nullptr;
    Pin* m_outForcePin = nullptr;
    ChannelBuffer m_outBuffer;
};

} // namespace nf
