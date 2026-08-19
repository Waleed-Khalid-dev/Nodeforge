#pragma once

#include "DataOp.h"

namespace nf {

class MergeDataOp : public DataOp {
public:
    MergeDataOp(NodeId id, const std::string& name);
    bool Cook(const CookContext& context) override;

private:
    Pin* m_inPin1 = nullptr;
    Pin* m_inPin2 = nullptr;
};

} // namespace nf
