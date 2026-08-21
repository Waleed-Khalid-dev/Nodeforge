#pragma once

#include "../../graph/Node.h"
#include "../../core/GeometryData.h"

namespace nf {

class ColliderGeomOp : public Node {
public:
    ColliderGeomOp(NodeId id, const std::string& name);
    ~ColliderGeomOp() override = default;

    bool Cook(const CookContext& context) override;

    glm::vec3 GetSize() const;
    glm::vec3 GetPosition() const;
    float GetRestitution() const;
    float GetFriction() const;

private:
    Pin* m_inGeomPin = nullptr;
    Pin* m_outGeomPin = nullptr;
    GeometryData m_outGeometry;
};

} // namespace nf
