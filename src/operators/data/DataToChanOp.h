#pragma once

#include "../chan/ChanOp.h"

namespace nf {

class DataToChanOp : public ChanOp {
public:
    DataToChanOp(NodeId id, const std::string& name);
    bool Cook(const CookContext& context) override;

private:
    Pin* m_inPin = nullptr;
};

} // namespace nf
