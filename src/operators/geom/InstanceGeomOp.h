#pragma once

#include "GeomOp.h"

namespace nf {

class InstanceGeomOp : public GeomOp {
public:
    InstanceGeomOp(NodeId id, const std::string& name);
    ~InstanceGeomOp() override = default;

    bool Cook(const CookContext& context) override;

private:
    Pin* m_inTemplatePin = nullptr;
    Pin* m_inDistMeshPin = nullptr;
    Pin* m_inDataPin = nullptr;
    Pin* m_inChanPin = nullptr;
};

} // namespace nf
