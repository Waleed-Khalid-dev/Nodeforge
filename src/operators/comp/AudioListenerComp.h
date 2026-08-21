#pragma once

#include "../../graph/Node.h"
#include "../../core/ChannelBuffer.h"

namespace nf {

class AudioListenerComp : public Node {
public:
    AudioListenerComp(NodeId id, const std::string& name);
    ~AudioListenerComp() override = default;

    bool Cook(const CookContext& context) override;

    glm::vec3 GetPosition() const;
    glm::vec3 GetOrientation() const;

private:
    Pin* m_inCamPin = nullptr;
    Pin* m_outCompPin = nullptr;
    Pin* m_outStatePin = nullptr;
    ChannelBuffer m_outBuffer;
};

} // namespace nf
