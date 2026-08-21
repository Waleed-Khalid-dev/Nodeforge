#pragma once

#include "../../graph/Node.h"
#include "../../core/ChannelBuffer.h"

namespace nf {

class OpenXRCameraComp : public Node {
public:
    OpenXRCameraComp(NodeId id, const std::string& name);
    ~OpenXRCameraComp() override = default;

    bool Cook(const CookContext& context) override;

    glm::vec3 GetCameraOffset() const;
    int32_t GetTrackingSpace() const;

private:
    Pin* m_outCompPin = nullptr;
    Pin* m_outHeadPosePin = nullptr;
    ChannelBuffer m_headPoseBuffer;
};

} // namespace nf
