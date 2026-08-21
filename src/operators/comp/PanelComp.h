#pragma once

#include "../../graph/Node.h"
#include "../../core/ChannelBuffer.h"
#include "../../gpu/Texture.h"
#include <memory>
#include <vector>

namespace nf {

class PanelComp : public Node {
public:
    PanelComp(NodeId id, const std::string& name);
    ~PanelComp() override = default;

    bool Cook(const CookContext& context) override;

    int32_t GetLayoutMode() const;
    glm::ivec2 GetPanelSize() const;

private:
    Pin* m_inUiPin = nullptr;
    Pin* m_inBgTexPin = nullptr;
    Pin* m_outCompPin = nullptr;
    Pin* m_outChanPin = nullptr;
    Pin* m_outTexPin = nullptr;

    ChannelBuffer m_outBuffer;
    std::shared_ptr<gpu::Texture2D> m_panelTexture;
};

} // namespace nf
