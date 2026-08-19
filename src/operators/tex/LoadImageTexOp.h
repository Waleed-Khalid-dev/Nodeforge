#pragma once

#include "TexOp.h"

namespace nf {

class LoadImageTexOp : public TexOp {
public:
    LoadImageTexOp(NodeId id, const std::string& name);
    virtual bool Cook(const CookContext& context) override;

private:
    std::string m_lastLoadedPath;
};

} // namespace nf
