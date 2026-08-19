#pragma once

#include "DataOp.h"

namespace nf {

class ChanToDataOp : public DataOp {
public:
    ChanToDataOp(NodeId id, const std::string& name);
    bool Cook(const CookContext& context) override;

private:
    Pin* m_inPin = nullptr;
};

} // namespace nf
