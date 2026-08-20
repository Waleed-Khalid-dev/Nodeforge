#pragma once

#include "MatOp.h"

namespace nf {

class ConstantMatOp : public MatOp {
public:
    ConstantMatOp(NodeId id, const std::string& name);
    bool Cook(const CookContext& context) override;

private:
    Pin* m_colorMapPin = nullptr;
};

} // namespace nf
