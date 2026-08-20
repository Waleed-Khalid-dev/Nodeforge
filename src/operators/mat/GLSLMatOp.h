#pragma once

#include "MatOp.h"

namespace nf {

class GLSLMatOp : public MatOp {
public:
    GLSLMatOp(NodeId id, const std::string& name);
    bool Cook(const CookContext& context) override;

private:
    Pin* m_map1Pin = nullptr;
    Pin* m_map2Pin = nullptr;
};

} // namespace nf
