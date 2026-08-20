#pragma once

#include "../../graph/Node.h"
#include "../../core/GeometryData.h"
#include "../mat/MatOp.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace nf {

class GeometryComp : public Node {
public:
    GeometryComp(NodeId id, const std::string& name);

    bool Cook(const CookContext& context) override;

    const GeometryData* GetGeometry() const;
    const MaterialData* GetMaterial() const;
    glm::mat4 GetTransformMatrix() const;
    std::vector<InstanceData> GetInstanceTransforms() const;

private:
    Pin* m_geomPin = nullptr;
    Pin* m_matPin = nullptr;
    Pin* m_instPin = nullptr;
    Pin* m_outPin = nullptr;

    GeometryData m_fallbackGeometry;
    MaterialData m_fallbackMaterial;
};

} // namespace nf
