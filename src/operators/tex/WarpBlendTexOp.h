#pragma once

#include "TexOp.h"
#include "../../media/WarpMesh.h"
#include "../../render/WarpBlendPass.h"

namespace nf {

class WarpBlendTexOp : public TexOp {
public:
    WarpBlendTexOp(NodeId id, const std::string& name);
    bool Cook(const CookContext& context) override;

    WarpMesh& GetWarpMesh() { return m_warpMesh; }
    const WarpMesh& GetWarpMesh() const { return m_warpMesh; }

private:
    Pin* m_inPin = nullptr;
    Pin* m_maskPin = nullptr;

    WarpMesh m_warpMesh;
    std::unique_ptr<WarpBlendPass> m_pass;
};

} // namespace nf
