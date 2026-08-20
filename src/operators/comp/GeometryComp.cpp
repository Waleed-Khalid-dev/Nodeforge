#include "GeometryComp.h"
#include "../../core/ChannelBuffer.h"
#include "../../core/DataTable.h"
#include <glm/gtc/quaternion.hpp>

namespace nf {

GeometryComp::GeometryComp(NodeId id, const std::string& name)
    : Node(id, name, "GeometryComp") {
    m_geomPin = AddInputPin("geometry", PinType::Geom);
    m_matPin = AddInputPin("material", PinType::Mat);
    m_instPin = AddInputPin("instances", PinType::Any);
    m_outPin = AddOutputPin("output", PinType::Any);

    SetParam("translate", glm::vec3(0.0f));
    SetParam("rotate", glm::vec3(0.0f));
    SetParam("scale", glm::vec3(1.0f));
    SetParam("pivot", glm::vec3(0.0f));
    SetParam("enable_instancing", false);

    m_fallbackGeometry = GeometryData::CreateBox();
}

bool GeometryComp::Cook(const CookContext& /*context*/) {
    return true;
}

const GeometryData* GeometryComp::GetGeometry() const {
    if (m_geomPin && m_geomPin->GetValue().Is<GeometryData>()) {
        const auto& g = m_geomPin->GetValue().Get<GeometryData>();
        if (!g.IsEmpty()) return &g;
    }
    return &m_fallbackGeometry;
}

const MaterialData* GeometryComp::GetMaterial() const {
    if (m_matPin && m_matPin->IsConnected()) {
        Pin* src = m_matPin->GetConnectedSource();
        if (src && src->GetNode()) {
            auto* matOp = dynamic_cast<MatOp*>(src->GetNode());
            if (matOp) {
                return &matOp->GetMaterialData();
            }
        }
    }
    return &m_fallbackMaterial;
}

glm::mat4 GeometryComp::GetTransformMatrix() const {
    glm::vec3 t = GetParam("translate").Is<glm::vec3>() ? GetParam("translate").Get<glm::vec3>() : glm::vec3(0.0f);
    glm::vec3 r = GetParam("rotate").Is<glm::vec3>() ? GetParam("rotate").Get<glm::vec3>() : glm::vec3(0.0f);
    glm::vec3 s = GetParam("scale").Is<glm::vec3>() ? GetParam("scale").Get<glm::vec3>() : glm::vec3(1.0f);
    glm::vec3 p = GetParam("pivot").Is<glm::vec3>() ? GetParam("pivot").Get<glm::vec3>() : glm::vec3(0.0f);

    glm::mat4 mat(1.0f);
    mat = glm::translate(mat, t + p);
    mat = glm::rotate(mat, glm::radians(r.z), glm::vec3(0, 0, 1));
    mat = glm::rotate(mat, glm::radians(r.y), glm::vec3(0, 1, 0));
    mat = glm::rotate(mat, glm::radians(r.x), glm::vec3(1, 0, 0));
    mat = glm::scale(mat, s);
    mat = glm::translate(mat, -p);
    return mat;
}

std::vector<InstanceData> GeometryComp::GetInstanceTransforms() const {
    std::vector<InstanceData> instances;
    bool enableInstancing = GetParam("enable_instancing").Is<bool>() ? GetParam("enable_instancing").Get<bool>() : false;

    if (!enableInstancing || !m_instPin || !m_instPin->IsConnected()) {
        return instances;
    }

    const auto& pinVal = m_instPin->GetValue();
    if (pinVal.Is<ChannelBuffer>()) {
        const auto& buf = pinVal.Get<ChannelBuffer>();
        size_t count = buf.GetSampleCount();
        if (count > 0) {
            instances.reserve(count);
            for (size_t i = 0; i < count; ++i) {
                float tx = buf.GetSample("tx", i);
                float ty = buf.GetSample("ty", i);
                float tz = buf.GetSample("tz", i);
                float sx = buf.GetSample("sx", i);
                float sy = buf.GetSample("sy", i);
                float sz = buf.GetSample("sz", i);
                float rx = buf.GetSample("rx", i);
                float ry = buf.GetSample("ry", i);
                float rz = buf.GetSample("rz", i);
                float cr = buf.GetSample("r", i);
                float cg = buf.GetSample("g", i);
                float cb = buf.GetSample("b", i);
                float ca = buf.GetSample("a", i);

                if (sx == 0.0f && sy == 0.0f && sz == 0.0f) {
                    sx = sy = sz = 1.0f;
                }
                if (ca == 0.0f && cr == 0.0f && cg == 0.0f && cb == 0.0f) {
                    cr = cg = cb = ca = 1.0f;
                }

                glm::mat4 m(1.0f);
                m = glm::translate(m, glm::vec3(tx, ty, tz));
                m = glm::rotate(m, glm::radians(rz), glm::vec3(0, 0, 1));
                m = glm::rotate(m, glm::radians(ry), glm::vec3(0, 1, 0));
                m = glm::rotate(m, glm::radians(rx), glm::vec3(1, 0, 0));
                m = glm::scale(m, glm::vec3(sx, sy, sz));

                instances.push_back(InstanceData{
                    .transform = m,
                    .color = glm::vec4(cr, cg, cb, ca)
                });
            }
        }
    } else if (pinVal.Is<DataTable>()) {
        const auto& table = pinVal.Get<DataTable>();
        size_t count = table.GetRowCount();
        if (count > 0) {
            instances.reserve(count);
            int txCol = table.FindColIndex("tx");
            int tyCol = table.FindColIndex("ty");
            int tzCol = table.FindColIndex("tz");
            int crCol = table.FindColIndex("r");
            int cgCol = table.FindColIndex("g");
            int cbCol = table.FindColIndex("b");

            for (size_t r = 0; r < count; ++r) {
                float tx = (txCol >= 0) ? table.GetCellFloat(r, static_cast<size_t>(txCol), 0.0f) : 0.0f;
                float ty = (tyCol >= 0) ? table.GetCellFloat(r, static_cast<size_t>(tyCol), 0.0f) : 0.0f;
                float tz = (tzCol >= 0) ? table.GetCellFloat(r, static_cast<size_t>(tzCol), 0.0f) : 0.0f;
                float cr = (crCol >= 0) ? table.GetCellFloat(r, static_cast<size_t>(crCol), 1.0f) : 1.0f;
                float cg = (cgCol >= 0) ? table.GetCellFloat(r, static_cast<size_t>(cgCol), 1.0f) : 1.0f;
                float cb = (cbCol >= 0) ? table.GetCellFloat(r, static_cast<size_t>(cbCol), 1.0f) : 1.0f;

                glm::mat4 m(1.0f);
                m = glm::translate(m, glm::vec3(tx, ty, tz));
                instances.push_back(InstanceData{
                    .transform = m,
                    .color = glm::vec4(cr, cg, cb, 1.0f)
                });
            }
        }
    }

    return instances;
}

} // namespace nf
