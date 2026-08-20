#pragma once

#include "GeomOp.h"

namespace nf {

class MergeGeomOp : public GeomOp {
public:
    MergeGeomOp(NodeId id, const std::string& name);
    bool Cook(const CookContext& context) override;

private:
    Pin* m_inPin1 = nullptr;
    Pin* m_inPin2 = nullptr;
};

} // namespace nf
