#pragma once

#include "GeomOp.h"

namespace nf {

class TransformGeomOp : public GeomOp {
public:
    TransformGeomOp(NodeId id, const std::string& name);
    bool Cook(const CookContext& context) override;

private:
    Pin* m_inPin = nullptr;
};

} // namespace nf
