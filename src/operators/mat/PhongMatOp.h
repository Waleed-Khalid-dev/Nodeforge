#pragma once

#include "MatOp.h"

namespace nf {

class PhongMatOp : public MatOp {
public:
    PhongMatOp(NodeId id, const std::string& name);
    bool Cook(const CookContext& context) override;

private:
    Pin* m_diffuseMapPin = nullptr;
    Pin* m_normalMapPin = nullptr;
};

} // namespace nf
