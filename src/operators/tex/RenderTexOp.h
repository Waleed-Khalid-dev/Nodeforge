#pragma once

#include "TexOp.h"
#include "../../render/Scene3DPass.h"

namespace nf {

class RenderTexOp : public TexOp {
public:
    RenderTexOp(NodeId id, const std::string& name);
    bool Cook(const CookContext& context) override;

private:
    Pin* m_geo1Pin = nullptr;
    Pin* m_geo2Pin = nullptr;
    Pin* m_camPin = nullptr;
    Pin* m_lightPin = nullptr;

    std::unique_ptr<Scene3DPass> m_scenePass;
};

} // namespace nf
